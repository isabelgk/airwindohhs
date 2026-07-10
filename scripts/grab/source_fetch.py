import shutil
import subprocess
import tempfile
from dataclasses import dataclass
from pathlib import Path

UPSTREAM_SOURCE_SUBDIR = Path("plugins/LinuxVST/src")


@dataclass
class VersionPin:
    repo_url: str
    ref: str

    @classmethod
    def parse(cls, text: str) -> "VersionPin":
        repo_url, _, ref = text.strip().partition("@")
        if not repo_url or not ref:
            raise ValueError(f"Malformed version.txt contents: {text!r} (expected '<repo_url>@<sha>')")
        return cls(repo_url=repo_url, ref=ref)

    def format(self) -> str:
        return f"{self.repo_url}@{self.ref}"


def read_version_pin(version_file: Path) -> VersionPin:
    return VersionPin.parse(version_file.read_text())


def write_version_pin(version_file: Path, pin: VersionPin):
    version_file.write_text(pin.format() + "\n")


def clone_upstream_source(repo_url: str, ref: str, dest: Path) -> Path:
    """Shallow, sparse clone of the upstream airwindows repo at `ref` into `dest`.

    Returns the path containing one subdirectory per plugin (matching the
    layout Grabber expects), i.e. `dest / UPSTREAM_SOURCE_SUBDIR`.
    """
    dest.mkdir(parents=True, exist_ok=True)
    _run(["git", "init", "-q"], cwd=dest)
    _run(["git", "remote", "add", "origin", repo_url], cwd=dest)
    _run(["git", "fetch", "--depth", "1", "origin", ref], cwd=dest)
    _run(["git", "sparse-checkout", "set", str(UPSTREAM_SOURCE_SUBDIR)], cwd=dest)
    _run(["git", "checkout", "FETCH_HEAD"], cwd=dest)
    return dest / UPSTREAM_SOURCE_SUBDIR


def _run(args: list[str], cwd: Path):
    subprocess.run(args, cwd=cwd, check=True, capture_output=True, text=True)


class UpstreamSource:
    """Resolves the plugin source directory to hand to Grabber, either by
    cloning upstream at a pinned ref or by using a local escape-hatch path."""

    def __init__(self, airwindows_root: Path, temp_dir: Path = None, keep_temp: bool = False):
        self.airwindows_root = airwindows_root
        self._temp_dir = temp_dir
        self._keep_temp = keep_temp

    def cleanup(self):
        if self._temp_dir is not None and not self._keep_temp:
            shutil.rmtree(self._temp_dir, ignore_errors=True)

    @classmethod
    def from_local_path(cls, path: Path) -> "UpstreamSource":
        return cls(airwindows_root=path)

    @classmethod
    def from_clone(cls, repo_url: str, ref: str, keep_temp: bool = False) -> "UpstreamSource":
        temp_dir = Path(tempfile.mkdtemp(prefix="airwindohhs-grab-"))
        airwindows_root = clone_upstream_source(repo_url, ref, temp_dir)
        return cls(airwindows_root=airwindows_root, temp_dir=temp_dir, keep_temp=keep_temp)
