#include <gtest/gtest.h>
#include <string>
#include <vector>
#include <fstream>
#include <cstdio>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <regex>
#include <thread>
#include <chrono>

std::pair<std::string, std::string> runParentProcess(const std::vector<std::string>& inputs) {
    // Create temporary file names
    std::string tempfile1 = "test_output1.txt";
    std::string tempfile2 = "test_output2.txt";

    // Run the parent process
    FILE* pipe = popen("./parent > /dev/null 2>&1", "w");
    fprintf(pipe, "%s\n%s\n", tempfile1.c_str(), tempfile2.c_str());
    for (const auto& input : inputs) {
        fprintf(pipe, "%s\n", input.c_str());
        fflush(pipe);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    fprintf(pipe, "\n");  // Send empty newline to signal exit
    fflush(pipe);
    pclose(pipe);

    // Read the contents of the files
    std::ifstream file1(tempfile1);
    std::ifstream file2(tempfile2);
    
    std::string output1((std::istreambuf_iterator<char>(file1)), std::istreambuf_iterator<char>());
    std::string output2((std::istreambuf_iterator<char>(file2)), std::istreambuf_iterator<char>());

    // Print file contents for debugging
    // std::cout << "File 1 contents:\n" << output1 << std::endl;
    // std::cout << "File 2 contents:\n" << output2 << std::endl;

    // Clean up temporary files
    remove(tempfile1.c_str());
    remove(tempfile2.c_str());

    return std::make_pair(output1, output2);
}

bool lineExists(const std::string& output, const std::string& expected) {
    std::istringstream iss(output);
    std::string line;
    while (std::getline(iss, line)) {
        if (line == expected) {
            return true;
        }
    }
    return false;
}

TEST(ParentChildTest, FilterStringsByLength) {
    std::vector<std::string> inputs = {
        "short", "this is a long string", "medium str",
        "another long string here", "tiny"
    };

    auto outputs = runParentProcess(inputs);
    const std::string& output1 = outputs.first;
    const std::string& output2 = outputs.second;

    EXPECT_TRUE(lineExists(output1, "shrt")) << "output1 does not contain 'shrt'";
    EXPECT_TRUE(lineExists(output1, "tny")) << "output1 does not contain 'tny'";
    EXPECT_TRUE(lineExists(output2, "ths s  lng strng")) << "output2 does not contain 'ths s  lng strng'";
    EXPECT_TRUE(lineExists(output2, "mdm str")) << "output2 does not contain 'mdm str'";
    EXPECT_TRUE(lineExists(output2, "nthr lng strng hr")) << "output2 does not contain 'nthr lng strng hr'";
}

TEST(ParentChildTest, HandleFiveStrings) {
    std::vector<std::string> inputs = {
        "one", "two", "three", "four", "five"
    };

    auto outputs = runParentProcess(inputs);
    const std::string& output1 = outputs.first;
    const std::string& output2 = outputs.second;

    int total_lines = std::count(output1.begin(), output1.end(), '\n') +
                      std::count(output2.begin(), output2.end(), '\n');
    EXPECT_EQ(total_lines, 5) << "Total lines is not 5";
    EXPECT_TRUE(lineExists(output1, "n")) << "output1 does not contain 'n'";
    EXPECT_TRUE(lineExists(output1, "tw")) << "output1 does not contain 'tw'";
    EXPECT_TRUE(lineExists(output1, "thr")) << "output1 does not contain 'thr'";
    EXPECT_TRUE(lineExists(output1, "fr")) << "output1 does not contain 'fr'";
    EXPECT_TRUE(lineExists(output1, "fv")) << "output1 does not contain 'fv'";
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}