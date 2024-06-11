#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <chrono>
#include <cstdlib>

bool patternIsEmpty(const std::vector<bool> &vec)
{
    for (bool item : vec)
    {
        if (item == true)
        {
            return false;
        }
    }
    return true;
}

class Node
{
public:
    int support;
    std::vector<bool> pattern;
    std::vector<Node> children;

    Node();
    ~Node();
};

Node::Node() : support(0), pattern(), children() {}

Node::~Node() {}

class CP_Miner
{
public:
    int min_support;
    std::map<int, int> unique;
    std::vector<std::vector<int> > rows;
    std::vector<std::vector<bool> > preprocessed;
    std::vector<int> frequent; // also legend for bit map
    Node parent;

    CP_Miner();
    void print_pattern(std::vector<bool> &pattern);
    void load_data(std::string file);
    void preprocess_dataset();
    void create_init_nodes();
    bool check_if_colossal(std::vector<bool> &pattern, std::vector<std::vector<bool> > &colossal_patterns);
    void cp_miner_procedure(Node &parent_node, std::vector<std::vector<bool> > &colossal_patterns);
    void run(std::string file, int min_support);
};

CP_Miner::CP_Miner() : min_support(0), unique(), rows(), preprocessed(), frequent(), parent() {}

void CP_Miner::load_data(std::string file_name)
{
    std::ifstream file(file_name);
    if (!file.is_open())
    {
        std::cerr << "Error opening file" << std::endl;
        return;
    }

    std::string line;
    while (std::getline(file, line))
    {
        std::istringstream ss(line);
        std::string str_item;
        int item;
        std::vector<int> row;

        while (std::getline(ss, str_item, ' '))
        {
            item = std::stoi(str_item);
            row.push_back(item);
            if (unique.find(item) == unique.end())
            {
                this->unique[item] = 1;
            }
            else
            {
                ++this->unique[item];
            }
        }
        this->rows.push_back(row);
    }
    file.close();
}

void CP_Miner::preprocess_dataset()
{
    std::vector<std::vector<int> > after_preprocessing;

    for (std::vector<int> row : this->rows)
    {
        std::vector<int> preprocessed_row;
        for (int item : row)
        {
            if (this->unique[item] >= this->min_support)
            {
                preprocessed_row.push_back(item);
            }
        }
        if (preprocessed_row.size() != 0)
        {
            std::sort(preprocessed_row.begin(), preprocessed_row.end());
            after_preprocessing.push_back(preprocessed_row);
        }
    }
    // release memory since we don't need original rows anymore
    this->rows.clear();

    // find frequent elements
    for (const std::pair<int, int> &entry : this->unique)
    {
        if (entry.second >= this->min_support)
        {
            this->frequent.push_back(entry.first);
        }
    }
    std::sort(this->frequent.begin(), this->frequent.end());
    size_t max_lenght = std::max(this->frequent.size(), static_cast<size_t>(0));
    std::map<int, int> map_bits;
    for (size_t i = 0; i < frequent.size(); ++i)
    {
        map_bits[frequent[i]] = i;
    }
    // turn vector of vectors of ints into vector of binary vectors
    for (std::vector<int> row : after_preprocessing)
    {
        std::vector<bool> pattern(max_lenght, false);
        for (int item : row)
        {
            pattern[map_bits[item]] = true;
        }
        this->preprocessed.push_back(pattern);
    }
}

void CP_Miner::create_init_nodes()
{
    for (std::vector<bool> row : this->preprocessed)
    {
        Node node;
        node.pattern = row;
        node.support = 1;
        this->parent.children.push_back(node);
    }
}

bool isSubset(const std::vector<bool> &a, const std::vector<bool> &b)
{
    for (size_t i = 0; i < a.size(); ++i)
    {
        if (a[i] && !b[i])
        {
            return false;
        }
    }

    return true;
}

bool CP_Miner::check_if_colossal(std::vector<bool> &pattern, std::vector<std::vector<bool> > &colossal_patterns)
{
    for (std::vector<bool> col_pattern : colossal_patterns)
    {
        if (isSubset(pattern, col_pattern))
        {
            return false;
        }
    }
    return true;
}

void CP_Miner::print_pattern(std::vector<bool> &pattern)
{
    for (size_t i = 0; i < pattern.size(); ++i)
    {
        if (pattern[i])
        {
            std::cout << this->frequent[i] << " ";
        }
    }
    std::cout << std::endl;
}

void CP_Miner::cp_miner_procedure(Node &parent_node, std::vector<std::vector<bool> > &colossal_patterns)
{
    if (parent_node.support == this->min_support - 1)
    {
        for (Node node : parent_node.children)
        {
            if (this->check_if_colossal(node.pattern, colossal_patterns))
            {
                colossal_patterns.push_back(node.pattern);
            }
        }
    }
    else
    {
        for (size_t i = 0; i < parent_node.children.size() - 1; ++i)
        {
            if ((parent_node.children[i].support + parent_node.children.size() - i - 1 >= this->min_support) && !patternIsEmpty(parent_node.children[i].pattern))
            {
                for (size_t j = i + 1; j < parent_node.children.size(); ++j)
                {
                    std::vector<bool> child_pattern(parent_node.children[i].pattern.size(), false);
                    bool patternNotEmpty = false;
                    for (size_t k = 0; k < child_pattern.size(); ++k)
                    {
                        child_pattern[k] = parent_node.children[i].pattern[k] & parent_node.children[j].pattern[k];
                        patternNotEmpty = patternNotEmpty || child_pattern[k];
                    }
                    if (patternNotEmpty)
                    {
                        Node child;
                        child.pattern = child_pattern;
                        child.support = parent_node.children[i].support + 1;
                        parent_node.children[i].children.push_back(child);
                        if (child_pattern == parent_node.children[j].pattern)
                        {
                            for (size_t l = 0; l < parent_node.children[j].pattern.size(); ++l)
                            {
                                parent_node.children[j].pattern[l] = false;
                            }
                        }
                    }
                }
                if (parent_node.children[i].children.size() > 0)
                {
                    this->cp_miner_procedure(parent_node.children[i], colossal_patterns);
                }
            }
        }
    }
}

void CP_Miner::run(std::string file, int min_support)
{
    this->load_data(file);
    this->min_support = min_support;
    auto start = std::chrono::high_resolution_clock::now();
    this->preprocess_dataset();
    this->create_init_nodes();
    std::vector<std::vector<bool> > colossal_patterns;
    this->cp_miner_procedure(this->parent, colossal_patterns);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    for (std::vector<bool> col_pattern : colossal_patterns)
    {
        this->print_pattern(col_pattern);
    }
    std::cout << "Execution time: " << duration / 1000 << "." << duration % 1000 << " s" << std::endl;
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        std::cerr << "Usage: " << argv[0] << " <path to csv file> <minimal support>" << std::endl;
        return 1;
    }

    std::string path = argv[1];
    int min_support = std::atoi(argv[2]);

    CP_Miner cp_miner;
    cp_miner.run(path, min_support);
    return 0;
}
