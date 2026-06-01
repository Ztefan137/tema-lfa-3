#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <set>
#include <map>

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

bool capital_letters(std::vector<std::string> letters){
    for(const auto& s:letters){
        if(s.size()!=1){
            return false;
        }
        char c=s[0];
        if(c<'A'||c>'Z'){
            return false;
        }
    }
    return true;
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
        std::set<production> find(std::string type){
        std::set<production> temp;
        if(type == "null_productions"){
            for(auto production:productions){
                if((production.characters[0] == "λ") && production.characters.size() == 1){
                    temp.insert(production);
                }
            }
        }
        if(type == "unit_productions"){
            for(auto production:productions){
                if(('A' <= production.characters[0][0] && production.characters[0][0] <= 'Z') && production.characters.size() == 1){
                    temp.insert(production);
                }
            }
        }
        else if(type == "long_productions"){
            for(auto production:productions){
                if(production.characters.size() >=3){
                    temp.insert(production);
                }
            }
        }
        else if(type == "binary_terminals"){
            for(auto production:productions){
                if(production.characters.size() == 2){
                    if(production.characters[0][0] < 'A' || production.characters[0][0] > 'Z'){
                        temp.insert(production);
                    }
                    if(production.characters[1][0] < 'A' || production.characters[1][0] > 'Z'){
                        temp.insert(production);
                    }
                }
            }
        }
        return temp;
    }
    std::set<production> determine_letter_productions(std::string letter){
        std::set<production> temp;
        for(auto production:productions){
            if(production.init == letter){
                temp.insert(production);
            }
        }
        return temp;
    }
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
    }
    void print(){
        for(auto production:productions){
            production.print();
        }
    }
    void eliminate_lambda_productions(){
        std::set<production> null_productions=find("null_productions");
        std::set<std::string> lambda_non_terminals;
        for(auto production:null_productions){
            lambda_non_terminals.insert(production.init);
        }
        for(auto production:productions){
            for(auto non_terminal:lambda_non_terminals){

            }
        }
        //TO DO
    }
    void eliminate_self_productions(){
        std::set<production> unit_productions=this->find("unit_productions");
        for(auto production:unit_productions){
            if(production.init == production.characters[0]){
                productions.erase(production);
            }
        }
    }
    void eliminate_unit_productions() {
        while(this->find("unit_productions").size()>0){
            eliminate_self_productions();
            std::set<production> unit_productions = this->find("unit_productions");
            for (auto curr_production : unit_productions) {
                std::set<production> letter_productions = determine_letter_productions(curr_production.characters[0]);
                for (auto letter_production : letter_productions) {
                    productions.insert({curr_production.init, letter_production.characters});
                }
                productions.erase(curr_production);
            }
        }
    }
    void eliminate_inaccesible_productions(){
        //TO DO
    }
    void eliminate_long_productions() {
        std::set<production> long_productions = find("long_productions");
        int cnt= 1;
        char c = 'M';
        for (auto production : long_productions) {
            productions.erase(production);
            std::string current_init = production.init;
            const auto& chars = production.characters;
            int n = chars.size();
            for (int i=0;i<n-2;i++) {
                std::string next_non_terminal = std::string(1,c) + std::to_string(cnt++);
                productions.insert({current_init, {chars[i], next_non_terminal}});
                current_init = next_non_terminal;
            }
            productions.insert({current_init, {chars[n - 2], chars[n - 1]}});
            c++;
        }
    }
    void eliminate_binary_non_terminals(){
        std::set<production> binary_non_terminals_productions = find("binary_terminals");
        int terminal_counter = 1;
        std::map<std::string, std::string> terminal_association;
        
        for(const auto& terminal : terminals){
            terminal_association[terminal] = "T" + std::to_string(terminal_counter++);
        }
        
        std::map<std::string, bool> used_non_terminals;
        std::set<std::string> term_set(terminals.begin(), terminals.end());

        for(auto production : binary_non_terminals_productions){
            productions.erase(production);
            std::vector<std::string> new_chars = production.characters;

            for(int i = 0; i < 2; ++i) {
                if(term_set.count(new_chars[i])) {
                    std::string non_terminal_name = terminal_association[new_chars[i]];
                    if(!used_non_terminals[non_terminal_name]) {
                        productions.insert({non_terminal_name, {new_chars[i]}});
                        used_non_terminals[non_terminal_name] = true;
                    }
                    new_chars[i] = non_terminal_name;
                }
            }
            productions.insert({production.init, new_chars});
        }
    }
    void normalize(){
        //this->eliminate_lambda_productions();
        this->eliminate_unit_productions();
        this->eliminate_long_productions();
        this->eliminate_binary_non_terminals();
    }
};
int main(){
    grammar test_grammar;
    test_grammar.read("normalizare.txt");
    test_grammar.normalize();
    test_grammar.print();
    return 0;
}