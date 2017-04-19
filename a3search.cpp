#include<iostream>
#include<algorithm>
#include<dirent.h>
#include<string>
#include<fstream>
#include<sys/stat.h>
#include<vector>
#include<map>
#include<cstdlib>
/*
 void readFromDir(DIR*, string);
 char caseUniform(char);
 */
using namespace std;

int pNum;
int* fileResult; //used for record a file's pattern counting

map<string, float> mm; //used for lastly sorting

/*******SHIFT TABLE*****/
const int shiftSZ = 256*256;
int SHIFT[shiftSZ] = {0};

/*******PREFIX TABLE****/
map<string,int> PREFIX;

/*****HASH TABLE******/
vector<int> pattern_pos_list;
vector<string> pattern_list;
int HASH[shiftSZ] = {0};

int minPL; //get the minimum length of pattern value


/*
 *  used for lastly sorting (compare value of map)
 */
typedef pair<string, float> PAIR;

struct cmp //used for map value sort
{
    bool operator()(const PAIR& a, const PAIR& b) {
        if(a.second!=b.second){
            return a.second < b.second;
        }else{
            return a.first > b.first;
        }
    }
};

/*
 *  uiform case
 */
char caseUniform(char c)
{
    if((int)c >= 97 && (int)c <= 122){
        return (int)c-97+65;
    }
    else return c;
}

/*
 * initial SHIFT table and HASH table
 */
void initTable(int m)
{
    for(int i=0; i<shiftSZ; i++){
        SHIFT[i] = m;
        HASH[i] = -1;
    }
}

int hashToKey(const char* c)
{
    int a = c[0];
    int b = c[1];
    return a*256+b;
}



/*
 * use WM algorithm to search patterns
 * the count and record the times for each pattern when finished scanning a file
 */

void MultiPatternSearch(ifstream &fin, string fileName){
    int strIndex;

    strIndex = minPL-1;
    int key;
    
    string str;
    while(getline(fin,str)){
        strIndex = minPL-1;
        const char* buffer = str.c_str();
        
        while(strIndex<str.length()){
            if(buffer[strIndex-1]<0 or buffer[strIndex]<0){
                strIndex++;
                continue;
            }
            key = 256*caseUniform(buffer[strIndex-1])+caseUniform(buffer[strIndex]);
            if(SHIFT[key]==0){
                int current_prefix_key =256*(int)caseUniform(buffer[strIndex-minPL+1])+(int)caseUniform(buffer[strIndex-minPL+2]);
            
                int endpoint;
                if(HASH[key]+1 == pattern_pos_list.size()){
                    endpoint = pattern_list.size();
                }else{
                    endpoint = pattern_pos_list[HASH[key]+1];
                }
            
                for(int i=pattern_pos_list[HASH[key]]; i<endpoint; i++){
                    string current_p = pattern_list[i];
                    if(PREFIX[current_p]!=current_prefix_key){
                        continue;
                    }else{
                        bool left = true;
                        bool right = true;
                        if(minPL>4){
                            for(int j=2; j<minPL-2;j++){
                                if(caseUniform(current_p[j])!=caseUniform(buffer[strIndex-minPL+1+j])){
                                    left = false;
                                    break;
                                }
                            }
                        }
                        if(left){
                            for(int j=minPL; j<current_p.size(); j++){
                            
                                if(caseUniform(current_p[j])!=caseUniform(buffer[strIndex-minPL+1+j])){
                                    right = false;
                                    break;
                                }
                            }
                        
                        
                        }
                        if(left && right){   //find pattern
                            fileResult[i] += 1; //this i is respond to the pattern in the pattern list;
                        }
                    
                    }
                }
                strIndex++;
            }else{strIndex+=SHIFT[key];}
        }
    }
    
    //after finishing search a file we calculate the mean occurence of patterns
    
    float fcount=0;
    bool flag=false;
    for(int i=0; i<pNum; i++){
        if(fileResult[i]==0){
            flag = true;
        }
        fcount += fileResult[i];
        fileResult[i] = 0;
    }
    if(flag){
        fcount = 0;
    }else{
        fcount = fcount/pNum;
        mm[fileName] = fcount;
    }
    
}




void readFromDir(DIR* dir, string dirPath)
{
    struct dirent *ptr;
    string filePath;
    string fileName;

    while((ptr=readdir(dir))!=NULL){
        
        if(ptr->d_name[0] == '.') continue;
        
        fileName = ptr->d_name;
        filePath = dirPath + "/" +  ptr->d_name;
        
        ifstream fin(filePath.c_str());
        MultiPatternSearch(fin,fileName);
        
        fin.close();
    }
}

int main(int argc, char* argv[])
{
    string dirPath = argv[1];
    DIR* dir = opendir(argv[1]);
    
    string pattern;
    int plen;
    
    int offset; // pattern start position;
    if(argv[3][0]=='-' && argv[3][1]=='s'){
        pNum = argc-5;
        offset = 5;
    }else{
        pNum = argc-3;
        offset = 3;
    }
    
    fileResult= (int*)calloc(pNum, sizeof(int)); //used for record a file's pattern counting

    string temp;
    temp = argv[offset];
    
    minPL = temp.length();
    for(int i=0; i<pNum; i++){
        temp = argv[i+offset];
        if(temp.length()<minPL){
            minPL = temp.length();
        }
    }
    
    initTable(minPL-1);
    //build SHIFT table
    vector<int> block_list;  // store block whose shift is 0
    for(int i=0; i<pNum; i++){
        temp = argv[i+offset];
        const char* ttemp = temp.c_str();
        int block_key;
        int block_value;
        bool findkey;
        
        string pre="";
        pre=pre+(char)caseUniform(ttemp[0])+(char)caseUniform(ttemp[1]);
        //PREFIX[temp]＝pre;
        
        PREFIX.insert(pair <string,int> (temp, hashToKey(pre.c_str())));
        
        for(int j=0; j<minPL-1; j++){
            findkey = false;
            string block = "";
            block = block+caseUniform(ttemp[j])+caseUniform(ttemp[j+1]);
            block_key = hashToKey(block.c_str());
            block_value = minPL-2-j;
            if(block_value<SHIFT[block_key]){ //build shift table
                SHIFT[block_key] = block_value;
            }
            if(SHIFT[block_key]==0){
                block_list.push_back(block_key);
            }
        }
    }
    
    sort(block_list.begin(), block_list.end());
    //remove duplicated element
    vector<int>::iterator iter = unique(block_list.begin(),block_list.end());
    block_list.erase(iter,block_list.end());
    
    
    for(int i=0; i<block_list.size(); i++){
        HASH[block_list[i]]=i;
    }
    
    int pCount = 0;
    const char* tt;
    for(int i=0; i<block_list.size(); i++){
        pattern_pos_list.push_back(pCount);  // help to point from HASH TABLE to pattern list
        for(int j=0; j<pNum; j++){
            temp = argv[j+offset];
            tt = temp.c_str();
            if (caseUniform(tt[minPL-2])== block_list[i]/256 && caseUniform(tt[minPL-1])==block_list[i]%256){
                pattern_list.push_back(temp);  //pattern_pos_list contains patterns sorted by block whose SHIFT value is 0
                pCount++;
            }
        }
    }
    
    readFromDir(dir, dirPath);
   
    vector<PAIR> mm_vec(mm.begin(), mm.end());
    /*
     for (int i = 0; i != mm_vec.size(); i++) {
     if(mm_vec[i].second == 0) continue;
     cout<<(mm_vec[i]).first<<"-"<<(mm_vec[i]).second<<endl;
     }
     */
    
    sort(mm_vec.rbegin(), mm_vec.rend(), cmp());
    
    for (int i = 0; i != mm_vec.size(); i++) {
        if(mm_vec[i].second == 0) continue;
        cout<<(mm_vec[i].first)<<endl;
        //cout<<(mm_vec[i]).first<<" - "<<(mm_vec[i]).second<<endl;
    }
    
    closedir(dir);
    
    return 0;
}



