#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <algorithm>

using namespace std;

string TOKENS[] = {"ALPHABET","STATES","START","FINAL","TRANSITIONS","END"};
struct automata{
    vector<string> alphabet_v, states_v, start_v, final_v, transitions_v;
    vector<string>* getV(int i){
        switch (i)
        {
        case 0:
            return &alphabet_v;
            break;
        case 1:
            return &states_v;
            break;
        case 2:
            return &start_v;
            break;
        case 3:
            return &final_v;
            break;
        case 4:
            return &transitions_v;
            break;
        
        default:
            break;
        }
        return nullptr;
    }

    void print(){
        for(int i=0; i<5 ; i++){
            
            cout<<TOKENS[i]<<endl;
            for(const auto& s:*(getV(i))){
                cout << s << endl;
            }
        }
        cout<<TOKENS[5]<<endl;

    }
    ~automata(){}

};

int main(int argc, char const *argv[]){
    automata *NFA = new automata();
    automata *DFA = new automata();
    fstream file;
    string filename;

    if(argc == 2) filename = argv[1];
    else{
        cout<<"Enter the file name for the NFA: "<<endl;
        cin>>filename;
    }
    
    file.open(filename);
    string line;

    int i=0;
    while(getline(file,line)){
        line.erase(line.find_last_not_of(" \n\r\t")+1);
        if(line != TOKENS[i]){
            NFA->getV(i-1)->push_back(line);  
        }
        else
            i++;
    }
    
    //constructing DFA
    DFA->alphabet_v = NFA->alphabet_v;
    //since we are ignoring epsilon transformations, start state will be the same
    DFA->start_v = NFA->start_v;
    DFA->states_v.push_back(DFA->start_v.at(0));

    map<tuple<string,string>,bool> checker;

    for(int i = 0; i < DFA->states_v.size();i++){
        auto cState = DFA->states_v.at(i);
        for(const auto& letter:NFA->alphabet_v){
            string newState="";
            for(char ch:cState){
                string c = string(1,ch);
                for(const auto& nfastates:NFA->states_v){
                    string control = c+ " "+ letter+" "+nfastates;
                    if(newState.find(nfastates) == string::npos){
                        auto place = find(NFA->transitions_v.begin(), NFA->transitions_v.end(),control);
                        if(place != NFA->transitions_v.end()){
                            newState += nfastates;
                        }
                    }
                }
            }
            if(newState != ""){
                //sorting the state name so that traversing the same states with different order does not cause a new state
                sort(newState.begin(), newState.end());
                if(find(DFA->states_v.begin(), DFA->states_v.end(), newState) == DFA->states_v.end()){
                    DFA->states_v.push_back(newState);
                    for(const auto& f:NFA->final_v){
                        if(newState.find(f) != string::npos)
                            DFA->final_v.push_back(newState);
                    }
                }

                DFA->transitions_v.push_back(cState+" "+letter+" "+newState);
                checker.insert_or_assign(tuple<string,string>{cState, letter},true);
                // cout<<"added :"<<newState<<" "<<letter<<endl;
            }
        }
    }

    bool sinkNecessarry = false;
    for(const auto& state:DFA->states_v){
        for(const auto& letter:DFA->alphabet_v){
            auto it = checker.find(tuple<string,string>{state,letter})->second;
            
            if(!it){
                sinkNecessarry = true;
                DFA->transitions_v.push_back(state+" "+letter+" SINK_STATE");
            }
        }
    }

    if(sinkNecessarry){
        //now we need the sink state for unspecified transitions in nfa
        DFA->states_v.push_back("SINK_STATE");
        for(const auto& letter:DFA->alphabet_v){
        //     checker.insert({tuple<string,string>{"SINK_STATE", letter},false});
            DFA->transitions_v.push_back("SINK_STATE "+letter+" SINK_STATE");
        }
    }

    DFA->print();
    file.close();   
    delete NFA;
    delete DFA;
    return 0;
}