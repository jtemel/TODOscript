#include <sys/types.h>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>

using namespace std;

#define MAX_BLOCK 3
#define MAX_SINGLE 4
#define KEYWORD_COUNT 3

/*
 * Use this to check when a block comment starts. To check when the comment
 * ends, match the reverse of each string.
 */
std::string bMarkers[3] = { "/*", "'''", "\"\"\""};

/*
 * Use this to check when a single line comment starts. To check when the
 * comment ends, match anything but the initial match.
 */
std::string sMarkers[4] = { "//", "#", "<!-", "%" };

/*
 * Use this to locate when and where the _TODO or _FIXME begins.
 */
std::string keyword[3] = { "TODO", "FIXME", "OPTIMIZE" };

/* Formatted to display file names, which this will be prepended and appended */
std::string separator = "======";

/* Used to determine if the file header should be written. */
bool openned = false;

/* Name of the file that is being parsed. */
std::string filename = "";

/* How many TODO/FIXME blocks were parsed and written. */
int keyCount = 0;

/* The file to write the TODO/FIXMEs too */
std::ofstream todofile;

/* The name of the file refer to by todofile. */
static std::string todofilename = "";

/*
 * Notify the user that the program was called incorrectly, returning 1.
 */
int print_usage()
{
    fprintf(stderr, "[PARSER] Usage: TODOparser write_file\n");
    return 1;
}

/*
 * Determine if the string has a keyword in it, and if so, return the substring
 * starting at the keyword. If no keyword is found, an empty string is returned.
 */
std::string find_keyword(std::string line, int key)
{
    int tIndex;
    return (tIndex = line.find(keyword[key])) != std::string::npos?
            line.substr(tIndex) : "";
}

/*
 * If the marker to signify a block comment is found in the line,
 * return the index of the char after the marker. If nothing is found, -1 is 
 * returned.
 */
int block_marker(std::string line, bool end)
{
    for (int i = 0; i < MAX_BLOCK; i++)
    {
        std::string marker(bMarkers[i]);

        if (end)
            std::reverse(marker.begin(), marker.end());

        int loc = line.find(marker);

        if (loc != std::string::npos)
            return loc + marker.length();
    }
    return -1;
}

/*
 * Determine if the line contains a single lined comment marker, returning
 * index of char after the marker, or -1 if no marker is found.
 */
int single_marker(std::string line)
{
    int loc = -1;
    for (int i = 0; i < MAX_SINGLE; i++)
    {
        if ((loc = line.find(sMarkers[i])) != std::string::npos)
            return loc + sMarkers[i].length();
    }
    return -1;
}

/*
 * Determine if the line has a keyword, and if so, write the line to the 
 * file. If there exists a TODO/FIXME/OPTIMIZE in the file and this
 * is the first time openning the file, the file header will be written aswell.
 * Returns true if the line has a keyword (or if foundKey was true) and false 
 * otherwise.
 */
bool parse_line(std::string currline, int lineCount, int key, bool foundKey)
{
    std::string subline;
    if ((subline = find_keyword(currline, key)).compare("") != 0)
    {
        if (!todofile.is_open())
            todofile.open(todofilename.c_str(), ios::out | ios::app);

        if (!openned)
        {
            openned = true;
            todofile << separator << filename << separator << std::endl;
        }

        todofile << "(" << keyCount++ << "):" << endl;
        foundKey = true;
    }
    else
    {
        subline = currline;
    }
    if (foundKey)
        todofile << "\t[" << lineCount << "] " << subline << std::endl;
    return foundKey;
}

/* Initialize the foundKey array to false. */
void init_bool(bool *foundKey)
{
    for (int i = 0; i < KEYWORD_COUNT; i++)
        foundKey[i] = false;
}

/*
 * Check if any of the keywords are found in the current line, updating the
 * corresponding foundKey bool if applicable.
 */
void parse_keyword_line(std::string currline, int lineCount, bool *foundKey)
{
    for (int i = 0; i < KEYWORD_COUNT; i++)
        if (foundKey[i] = parse_line(currline, lineCount, i, foundKey[i]))
            return; 
}

/*
 * Given that the line is a comment, determine if it contains a keyword. 
 * Consecutive lines will be parsed until a non-single lined comment is found.
 * Returns the new line number.
 */
int parse_line_comment(std::string currline, int lineCount)
{
    if (currline.length() == 0)
        return ++lineCount;

    bool foundKey[KEYWORD_COUNT];
    init_bool(foundKey);
    int cmtIndex = 0;
    parse_keyword_line(currline, lineCount, foundKey);
    
    for (std::string line; std::getline(std::cin, line);)
    {
        lineCount++;
        if (single_marker(line) <  0)
            break;

        while ((cmtIndex = single_marker(line)) >= 0)
                line = line.substr(cmtIndex);

        parse_keyword_line(line, lineCount, foundKey);
    }
    return lineCount;
}

/*
 * Given that the line is the beginning of a block comment, scan each line until
 * the end of comment marker is found. If a line consists of a TODO/FIXME, sent
 * it to be parsed and written.
 */
int parse_block_comment(std::string currline, int lineCount)
{
    bool foundKey[KEYWORD_COUNT];
    init_bool(foundKey);
    if (currline.length() != 0 && block_marker(currline, true))
        return ++lineCount;

    for (std::string line; std::getline(std::cin, line);)
    {
        lineCount++;
        if (block_marker(line, true) > 0)
            break;

        parse_keyword_line(line, lineCount, foundKey);
    }
    return lineCount;
}

/*
 * Returns the number of keyWords found in the file. */
int main(int argc, char *argv[])
{
    if (argc != 3)
        return print_usage();
    
    filename = argv[1];
    todofilename = argv[2];
    todofile.open(todofilename.c_str(), ios::out | ios::app);
 
    if(!todofile.is_open())
        exit(1);


    int lineCount = 1;
    int startLine, endLine;
    int cmtIndex;

    for (std::string line; std::getline(std::cin, line);)
    {
        if ((cmtIndex = block_marker(line, false)) > 0)
        {
            lineCount = parse_block_comment(line.substr(cmtIndex), lineCount);
        }
        else if ((cmtIndex = single_marker(line)) >= 0)
        {
            lineCount = parse_line_comment(line.substr(cmtIndex), lineCount);
        }
        else
        {
            lineCount++;
        }   
    }

    return keyCount;
}
