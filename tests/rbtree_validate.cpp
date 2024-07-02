#include "RBTree.hpp"

#include <algorithm>
#include <cassert>
#include <format>
#include <future>
#include <iostream>
#include <map>
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

static constexpr int NUM_OF_COMMANDS_PER_TEST = 100'000;

enum class Command
{
    INSERT,
    INSERT_OR_ASSIGN,
    FIND_AND_ERASE,

    TOTAL_COUNT
};

struct ReproduceInfo
{
public:
    struct CommandInfo
    {
        Command cmd;
        int key;
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
        case Command::INSERT:
            os << "insert(" << cmd.key << ")\n";
            break;
        case Command::INSERT_OR_ASSIGN:
            os << "insert_or_assign(" << cmd.key << ")\n";
            break;
        case Command::FIND_AND_ERASE:
            os << "erase(" << cmd.key << ")\n";
            break;

        default:
            throw std::logic_error(std::format("Invalid command kind={}", (int)cmd.cmd));
        }
    }

    return os;
}

bool worker(unsigned seed);
bool validate(unsigned seed, int idx, const bs::RBTree<int, int>&, const std::map<int, int>&, const ReproduceInfo&);

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

    bs::RBTree<int, int> t;
    std::map<int, int> m;

    ReproduceInfo repro;

    TEST_ASSERT(t.empty() && m.empty());
    if (!validate(seed, idx, t, m, repro))
        return false;

    std::mt19937 rand(seed);
    std::uniform_int_distribution all_int_range;
    std::uniform_int_distribution command_range(0, (int)Command::TOTAL_COUNT - 1);

    for (idx = 0; idx < NUM_OF_COMMANDS_PER_TEST; ++idx)
    {
        const auto command_kind = (Command)command_range(rand);
        switch (command_kind)
        {
        case Command::INSERT: {
            const int num = all_int_range(rand);
            repro.commands.emplace_back(Command::INSERT, num);
            TEST_ASSERT(t.insert(num, num) == m.insert({num, num}).second, repro);
            break;
        }
        case Command::INSERT_OR_ASSIGN: {
            const int num = all_int_range(rand);
            repro.commands.emplace_back(Command::INSERT_OR_ASSIGN, num);
            TEST_ASSERT(t.insert_or_assign(num, num) == m.insert_or_assign(num, num).second, repro);
            break;
        }
        case Command::FIND_AND_ERASE:
            if (!t.empty())
            {
                // find a random `key` that exists inside of tree
                int key = 0;
                {
                    const std::size_t iter_pos = std::uniform_int_distribution<std::size_t>(0, m.size() - 1)(rand);
                    auto iter = m.begin();
                    for (std::size_t i = 0; i < iter_pos && iter != m.end(); ++i)
                        ++iter;

                    assert(iter != m.end());
                    key = iter->first;
                }

                repro.commands.emplace_back(Command::FIND_AND_ERASE, key);
                TEST_ASSERT(t.erase(key) == (bool)m.erase(key), repro);
            }
            break;

        default:
            throw std::logic_error(std::format("Invalid command kind={}", (int)command_kind));
        }
        if (!validate(seed, idx, t, m, repro))
            return false;
    }

    t.clear();
    m.clear();

    TEST_ASSERT(t.empty() && m.empty(), repro);
    if (!validate(seed, idx, t, m, repro))
        return false;

    return true;
}

bool validate(unsigned seed, int idx, const bs::RBTree<int, int>& t, const std::map<int, int>& m,
              const ReproduceInfo& repro)
{
    TEST_ASSERT(t.empty() == m.empty(), repro);
    TEST_ASSERT(t.size() == m.size(), "\t", t.size(), " - ", m.size(), "\n", repro);

    std::vector<int> t_res, m_res;
    t_res.reserve(t.size());
    m_res.reserve(m.size());

    t.inorder([&t_res]([[maybe_unused]] int key, int val, [[maybe_unused]] const bs::TraversalInfo& info) {
        t_res.push_back(val);
    });

    for (const auto [key, val] : m)
        m_res.push_back(val);

    TEST_ASSERT(t_res == m_res, repro);
    return true;
}
