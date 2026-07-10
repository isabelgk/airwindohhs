import shutil
import subprocess
import tempfile
from dataclasses import dataclass
from pathlib import Path

PLUGIN_SOURCE_SUBDIR = Path("plugins/LinuxVST/src")
AIRWINDOPEDIA_SUBPATH = Path("Airwindopedia.txt")


@dataclass
class VersionPin:
    repo_url: str
    ref: str

    @classmethod
    def parse(cls, text: str) -> "VersionPin":
        repo_url, _, ref = text.strip().partition("@")
        if not repo_url or not ref:
            raise ValueError(f"Malformed airwindows-version.txt contents: {text!r} (expected '<repo_url>@<sha>')")
        return cls(repo_url=repo_url, ref=ref)

    def format(self) -> str:
        return f"{self.repo_url}@{self.ref}"


def read_version_pin(version_file: Path) -> VersionPin:
    return VersionPin.parse(version_file.read_text())


def write_version_pin(version_file: Path, pin: VersionPin):
    version_file.write_text(pin.format() + "\n")


def clone_upstream_source(repo_url: str, ref: str, dest: Path) -> Path:
    """Shallow, sparse clone of the upstream airwindows repo at `ref` into `dest`.

    Checks out just the plugin sources (PLUGIN_SOURCE_SUBDIR) and
    Airwindopedia.txt -- the two things this generator reads -- so `dest`
    ends up matching the shape of a full local airwindows checkout. Returns
    `dest`.
    """
    dest.mkdir(parents=True, exist_ok=True)
    _run(["git", "init", "-q"], cwd=dest)
    _run(["git", "remote", "add", "origin", repo_url], cwd=dest)
    _run(["git", "fetch", "--depth", "1", "origin", ref], cwd=dest)
    _run(["git", "sparse-checkout", "set", str(PLUGIN_SOURCE_SUBDIR), str(AIRWINDOPEDIA_SUBPATH)], cwd=dest)
    _run(["git", "checkout", "FETCH_HEAD"], cwd=dest)
    return dest


def _run(args: list[str], cwd: Path):
    subprocess.run(args, cwd=cwd, check=True, capture_output=True, text=True)


class UpstreamSource:
    """Resolves the airwindows checkout root to read from -- either a fresh
    clone at a pinned ref, or a local escape-hatch path to an existing
    checkout -- and exposes the two locations Grabber needs within it."""

    def __init__(self, root: Path, temp_dir: Path = None, keep_temp: bool = False):
        self.root = root
        self._temp_dir = temp_dir
        self._keep_temp = keep_temp

    @property
    def plugin_source_dir(self) -> Path:
        return self.root / PLUGIN_SOURCE_SUBDIR

    @property
    def airwindopedia_path(self) -> Path:
        return self.root / AIRWINDOPEDIA_SUBPATH

    def cleanup(self):
        if self._temp_dir is not None and not self._keep_temp:
            shutil.rmtree(self._temp_dir, ignore_errors=True)

    @classmethod
    def from_local_path(cls, path: Path) -> "UpstreamSource":
        return cls(root=path)

    @classmethod
    def from_clone(cls, repo_url: str, ref: str, keep_temp: bool = False) -> "UpstreamSource":
        temp_dir = Path(tempfile.mkdtemp(prefix="airwindohhs-grab-"))
        root = clone_upstream_source(repo_url, ref, temp_dir)
        return cls(root=root, temp_dir=temp_dir, keep_temp=keep_temp)
