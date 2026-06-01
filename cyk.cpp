#include <iostream>
#include <vector>
#include <string>
#include <set>
#include <map>
#include <fstream>

std::vector<std::string> split_null(const std::string& s) {
    std::vector<std::string> result;
    if(s == "λ"){
        return {"λ"};
    }
    for (char c : s) {
        result.push_back(std::string(1,c));
    }
    return result;
}

std::vector<std::string> split(const std::string& s, char delim) {
    if(delim == '\0'){
        return split_null(s);
    }
    std::vector<std::string> result;
    size_t start = 0;
    size_t pos;
    while ((pos = s.find(delim, start)) != std::string::npos) {
        result.emplace_back(s.substr(start, pos - start));
        start = pos + 1;
    }
    result.emplace_back(s.substr(start));
    return result;
}

struct production{
    std::string init;
    std::vector<std::string> characters;
    production(std::string init,std::vector<std::string> characters) : init(init), characters(characters){

    }
    bool operator<(const production& other) const{
        if(init < other.init){
            return true;
        }
        if(init > other.init){
            return false;
        }
        return characters < other.characters;
    }
    void print(){
        std::cout<<init;
        std::cout<<"->";
        for(auto character:characters){
            std::cout<<character;
        }
        std::cout<<"\n";
    }
};

class grammar{
    std::vector<std::string> non_terminals;
    std::vector<std::string> terminals;
    std::string start;
    int production_count;
    std::set<production> productions;
public:
    void read(std::string input_file){
        std::ifstream file(input_file);
        std::string line;
        //neterminale
        std::getline(file,line);
        non_terminals=split(line,' ');
        //terminale
        std::getline(file,line);
        terminals=split(line,' ');
        //neterminale
        std::getline(file,line);
        production_count=std::stoi(line);
        for(int i=0;i<production_count;i++){
            std::getline(file,line);
            productions.insert({split(line,' ')[0],split(split(line,' ')[1],'\0')});
        }
        std::getline(file,line);
        start=line;
    }
    void print(){
        for(auto production:productions){
            production.print();
        }
    }
    bool check_word(std::string word) {
        int n = word.size();
        if (n == 0) return false;

        std::vector<std::vector<std::set<std::string>>> cyk_table(n, std::vector<std::set<std::string>>(n));

        for (int i = 0; i < n; i++) {
            std::string current_letter = std::string(1, word[i]);
            for (const auto& production : productions) {
                if (production.characters.size() == 1 && production.characters[0] == current_letter) {
                    cyk_table[i][i].insert(production.init);
                }
            }
        }

        for (int len=2;len<=n;len++) {
            for (int i=0;i<=n-len;i++) {
                int j=i+len-1;
                for (int m=i;m<j;m++) {
                    for (const auto& production : productions) {
                        if (production.characters.size() == 2) {
                            std::string left = production.characters[0];
                            std::string right = production.characters[1];

                            if (cyk_table[i][m].count(left) && cyk_table[m + 1][j].count(right)) {
                                cyk_table[i][j].insert(production.init);
                            }
                        }
                    }
                }
            }
        }
        return cyk_table[0][n - 1].count(start) > 0;
    }
};

int main(){
    grammar test_grammar;
    test_grammar.read("cyk.txt");
    std::ofstream file("cyk_output.txt");
    file<<test_grammar.check_word("baaba");
}