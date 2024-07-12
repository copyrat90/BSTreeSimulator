#include "AlterBinaryHeap.hpp"

#include <algorithm>
#include <cassert>
#include <format>
#include <future>
#include <iostream>
#include <random>
#include <sstream>
#include <stdexcept>
#include <thread>
#include <vector>

template <typename... Args>
void append_args(std::ostream& os, const Args&... args)
{
    if constexpr (sizeof...(args) > 0)
        (os << ... << args);
}

#define TEST_ASSERT(condition, ...) \
    do \
    { \
        if (!(condition)) \
        { \
            std::ostringstream oss; \
            oss << "Failed at seed=" << seed << ", idx=" << idx << ":\n"; \
            oss << "\t" << #condition << "\n"; \
            append_args(oss __VA_OPT__(, ) __VA_ARGS__); \
            oss << "\n\n"; \
            std::cerr << oss.str(); \
            return false; \
        } \
    } while (false)

static constexpr int NUM_OF_COMMANDS_PER_TEST = 1'000'000;

struct MyData
{
    int priority;
    int id;

    int unique_id() const
    {
        return id;
    }
    bool operator<(const MyData& other) const
    {
        return priority < other.priority;
    }
};

enum class Command
{
    PUSH,
    UPDATE,
    POP,

    TOTAL_COUNT
};

struct ReproduceInfo
{
public:
    struct CommandInfo
    {
        Command cmd;
        int key;
        int id;
    };

public:
    std::vector<CommandInfo> commands;

public:
    ReproduceInfo()
    {
        commands.reserve(NUM_OF_COMMANDS_PER_TEST);
    }
};

std::ostream& operator<<(std::ostream& os, const ReproduceInfo& repro)
{
    for (const auto& cmd : repro.commands)
    {
        switch (cmd.cmd)
        {
        case Command::PUSH:
            os << "push(key=" << cmd.key << ", id=" << cmd.id << ")\n";
            break;
        case Command::UPDATE:
            os << "update(key=" << cmd.key << ", id=" << cmd.id << ")\n";
            break;
        case Command::POP:
            os << "pop(key=" << cmd.key << ", id=" << cmd.id << ")\n";
            break;

        default:
            throw std::logic_error(std::format("Invalid command kind={}", (int)cmd.cmd));
        }
    }

    return os;
}

bool worker(unsigned seed);
bool validate(unsigned seed, int idx, const bs::AlterBinaryHeap<MyData>&, const ReproduceInfo&);

int main()
{
    unsigned cores = std::thread::hardware_concurrency();
    if (cores)
        std::cout << "system cores: " << cores << "\n";
    else
    {
        cores = 8;
        std::cout << "system cores detection failed, default to 8 cores\n";
    }

    std::vector<std::future<bool>> futures;
    std::vector<bool> results;

    futures.reserve(cores);
    results.reserve(cores);

    std::random_device rd;

    for (unsigned i = 0; i < cores; ++i)
        futures.push_back(std::async(std::launch::async, worker, rd()));

    for (unsigned i = 0; i < cores; ++i)
        results.push_back(futures[i].get());

    if (!std::ranges::all_of(results, [](const bool val) { return val; }))
        return -1;

    std::cout << "Test succeeded!\n";
    return 0;
}

bool worker(unsigned seed)
{
    // print current thread & seed info
    {
        std::ostringstream worker_info;
        worker_info << "TID #" << std::this_thread::get_id() << ": seed=" << seed << "\n";
        std::cout << worker_info.str();
    }

    int idx = -1;

    bs::AlterBinaryHeap<MyData> h;

    ReproduceInfo repro;

    TEST_ASSERT(h.empty());
    if (!validate(seed, idx, h, repro))
        return false;

    std::mt19937 rand(seed);
    std::uniform_int_distribution all_int_range;
    std::uniform_int_distribution command_range(0, (int)Command::TOTAL_COUNT - 1);

    for (idx = 0; idx < NUM_OF_COMMANDS_PER_TEST; ++idx)
    {
        const auto command_kind = h.empty() ? Command::PUSH : (Command)command_range(rand);
        switch (command_kind)
        {
        case Command::PUSH: {
            const int num = all_int_range(rand);
            repro.commands.emplace_back(Command::PUSH, num, -idx);
            h.push(MyData(num, idx));
            break;
        }
        case Command::UPDATE: {
            const std::size_t prev_size = h.size();

            std::uniform_int_distribution<std::size_t> select_range(0, h.size() - 1);
            const std::size_t selected_idx = select_range(rand);
            const int selected_id = h.begin()[selected_idx].id;

            const int num = all_int_range(rand);
            repro.commands.emplace_back(Command::UPDATE, num, selected_id);
            h.push(MyData(num, selected_id));
            TEST_ASSERT(prev_size == h.size());
            break;
        }
        case Command::POP: {
            repro.commands.emplace_back(Command::POP);
            h.pop();
            break;
        }

        default:
            throw std::logic_error(std::format("Invalid command kind={}", (int)command_kind));
        }
        if (!validate(seed, idx, h, repro))
            return false;
    }

    return true;
}

bool validate(unsigned seed, int idx, const bs::AlterBinaryHeap<MyData>& h, const ReproduceInfo& repro)
{
    TEST_ASSERT(h.validate(), repro);
    return true;
}
