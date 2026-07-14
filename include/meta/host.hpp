#pragma once
#include <cstddef>
#include <memory>
#include <string_view>
#include <vector>

#include "meta/iplugin.hpp"
#include "meta/plugin_entry.hpp"
#include "meta/registry.hpp"

namespace airwindohhs::meta {

// A "meta plugin": holds whichever concrete plugin is currently loaded and
// lets a caller swap it out at runtime by index or by name, forwarding
// process()/parameter calls to whatever's active.
template <typename T>
class PluginHost
{
  public:
    using Entry = PluginEntry<T>;

    static const std::vector<Entry>& all_plugins()
    {
        return airwindohhs::meta::all_plugins<T>();
    }

    bool load_by_index(std::size_t index)
    {
        const auto& table = all_plugins();
        if (index >= table.size()) {
            return false;
        }
        current_ = table[index].make();
        current_entry_ = &table[index];
        current_->setSampleRate(sample_rate_);
        return true;
    }

    bool load_by_name(std::string_view name)
    {
        const auto& table = all_plugins();
        for (std::size_t i = 0; i < table.size(); ++i) {
            if (table[i].name == name) {
                return load_by_index(i);
            }
        }
        return false;
    }

    void setSampleRate(T sampleRate)
    {
        sample_rate_ = sampleRate;
        if (current_) {
            current_->setSampleRate(sample_rate_);
        }
    }

    T getSampleRate() const
    {
        return sample_rate_;
    }

    IPlugin<T>* current()
    {
        return current_.get();
    }

    const Entry* current_entry() const
    {
        return current_entry_;
    }

    void process(T** inputs, T** outputs, long sampleFrames)
    {
        if (current_) {
            current_->process(inputs, outputs, sampleFrames);
        }
    }

    void set_parameter_value(int index, float value)
    {
        if (current_) {
            current_->set_parameter_value(index, value);
        }
    }

    float get_parameter_value(int index)
    {
        return current_ ? current_->get_parameter_value(index) : 0.0f;
    }

    T get_parameter_default(int index)
    {
        return current_ ? current_->get_parameter_default(index) : T{};
    }

    int num_parameters() const
    {
        return current_entry_ ? current_entry_->num_parameters : 0;
    }

  private:
    std::unique_ptr<IPlugin<T>> current_;
    const Entry* current_entry_ = nullptr;
    T sample_rate_{};
};

} // namespace airwindohhs::meta
