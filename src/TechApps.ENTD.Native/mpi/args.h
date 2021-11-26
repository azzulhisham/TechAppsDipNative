#include <iostream>
#include <cstdarg>
#include <vector>

// https://stackoverflow.com/questions/865668/parsing-command-line-arguments-in-c
class InputParser {
    public:
        InputParser (int &argc, char **argv){
            for (int i=1; i < argc; ++i)
                this->tokens.push_back(std::string(argv[i]));
        }
        /// @author iain
        const std::string& getCmdOption(const std::string &option) const{
            std::vector<std::string>::const_iterator itr;
            itr =  std::find(this->tokens.begin(), this->tokens.end(), option);
            if (itr != this->tokens.end() && ++itr != this->tokens.end()){
                return *itr;
            }
            static const std::string empty_string("");
            return empty_string;
        }
        /// @author iain
        bool cmdOptionExists(const std::string &option) const{
            return std::find(this->tokens.begin(), this->tokens.end(), option)
                   != this->tokens.end();
        }
    private:
        std::vector <std::string> tokens;
};


std::string format(const std::string& format, ...)
{
    va_list args;
    va_start(args, format);
    size_t len = std::vsnprintf(NULL, 0, format.data(), args);
    va_end(args);
    std::vector<char> vec(len + 1);
    va_start(args, format);
    std::vsnprintf(vec.data(), len + 1, format.data(), args);
    va_end(args);
    return vec.data();
}

void logger(const std::string& msg, int type, int verbosity = 0) {
    if (type > verbosity)
        return;
    std::string t;
    switch (type) {
    case 2: t = "INFO"; break;
    case 1: t = "WARNING"; break;
    case 0: t = "ERROR"; break;
    default: t = "";
    }
    printf("%s: %s\n", &t[0], &msg[0]);
}