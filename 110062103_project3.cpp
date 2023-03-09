#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <array>
#include <map>
#include <vector>
#include <climits>
#include <random>
#include <algorithm>
using namespace std; 
unsigned long long int ZobristTable[15][15][2];
unsigned long long  Hash;
mt19937 mt(01234567);
int statusboard[15][15][4];
int computepoint(int x, int y,bool ismax);
map <unsigned long long int,int> history;
enum p_status{Space,p_dead,p_dead_one, p_live_one, p_dead_two, p_live_two,p_dead_three,p_live_three,p_dead_four,p_live_four,p_FIVE};
enum b_status{SSpace,b_dead,b_dead_one, b_live_one, b_dead_two, b_live_two,b_dead_three,b_live_three,b_dead_four,b_live_four,b_FIVE};
vector<string> st= {"p_dead","p_dead_one", "p_live_one", "p_dead_two", "p_live_two","p_dead_three","p_live_three","p_dead_four","p_live_four",
                 "p_FIVE","b_dead","b_dead_one", "b_live_one", "b_dead_two", "b_live_two","b_dead_three","b_live_three","b_dead_four","b_live_four","b_FIVE"} ;
enum SPOT_STATE {
    EMPTY = 0,
    BLACK = 1,
    WHITE = 2
};
int player;
struct pnt{
    int x,y,value;
    pnt(int x, int y,int value):x(x),y(y),value(value){}
};
const int SIZE = 15;
std::array<std::array<int, SIZE>, SIZE> board;
int playercount=0;
unsigned long long int randomInt()
{
    uniform_int_distribution<unsigned long long int>
                                 dist(0, UINT64_MAX);
    return dist(mt);
}
unsigned long long int computeHash()
{
    unsigned long long int h = 0;
    for (int i = 0; i<15; i++)
    {
        for (int j = 0; j<15; j++)
        {
            if (board[i][j]!=EMPTY)
            {
                h ^= ZobristTable[i][j][board[i][j]/2];
            }
        }
    }
    return h;
}

int pvalue[10]= {25,125,100,500,400,1200,2000,6000};
int bvalue[10]= {50,300,400,1500,1500,5000,15000,150000};
void printstatus();
bool isconnect(int x,int y);
int alphabeta(int x,int y,int depth, int alpha,int beta, bool ismax);
int calculatevalue(bool isMax);
void calculate();
void initTable()
{
    for (int i = 0; i<8; i++)
      for (int j = 0; j<8; j++)
        for (int k = 0; k<12; k++)
          ZobristTable[i][j][k] = randomInt();
}
void read_board(std::ifstream& fin) {
    fin >> player;
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            fin >> board[i][j];
            if(board[i][j]==player) playercount=1;
        }
    }
    Hash =computeHash();
    calculate();
   // printstatus();
}

bool largetosmall(pnt a, pnt b){ return a.value>b.value;}

void nextmove(std::ofstream& fout){
    vector<pnt> cad;
    int value = INT_MIN,x=-1,y=-1;
    for(int i=0;i<SIZE;i++){
        for(int j=0;j<SIZE;j++){
            if(!board[i][j] && isconnect(i,j)){
                 int tp = computepoint(i,j,true);
                 cad.push_back(pnt(i,j,tp));
            }
        }
    }
    sort(cad.begin(),cad.end(),largetosmall);
    int xx = cad.size();
    for(int i=0; i<min(10,xx);i++){
        int ab = alphabeta(cad[i].x,cad[i].y,6,INT_MIN,INT_MAX,true);
       // cout << "point(" << cad[i].x << "," <<cad[i].y<<") suppose value is " << cad[i].value << ", actual value is " << ab << "\n";
        if(value <= ab){
            value = ab;
            x = cad[i].x,y = cad[i].y;
        }
    }
    if(x==-1 && y==-1) {
        if(board[SIZE/2][SIZE/2]==0)
        x= SIZE/2;
        y= SIZE/2;
    }
     fout << x << " " << y << " " << value << std::endl;
    
    fout.flush();
}
bool iswin(int x,int y);
int alphabeta(int x,int y,int depth, int alpha,int beta, bool ismax){
    int value;
    board[x][y] = ismax? player:3-player;
    if(iswin(x,y)){
        board[x][y]=0;
        return ismax? INT_MAX:INT_MIN;
    }
    if(depth==0){
           Hash ^= ZobristTable[x][y][board[x][y]/2];
           if(history.find(Hash)==history.end()){
            value = calculatevalue(ismax);
           history[Hash] = value;
           } 
           board[x][y]=0;
           return history[Hash];
        }
    vector<pnt> cad;
    for(int i=0;i<SIZE;i++){
        for(int j=0;j<SIZE;j++){
            if(!board[i][j] && isconnect(i,j)){
                 int tp = computepoint(i,j,!ismax);
                 cad.push_back(pnt(i,j,tp));
            }
        }
    }
    sort(cad.rbegin(),cad.rend(),largetosmall);
    int xx= cad.size();
    int playervalue=INT_MIN,basevalue= INT_MAX;
    for(int i=0;i<min(10,xx);i++){
        if(ismax){
                 int ab = alphabeta(cad[i].x,cad[i].y,depth-1,alpha,beta,false);
                 playervalue = playervalue > ab? playervalue:ab;
                 alpha = alpha>playervalue?alpha:playervalue;
                 if(alpha>=beta) break;  
               } else{
                 int ab = alphabeta(cad[i].x,cad[i].y,depth-1,alpha,beta,true);
                 basevalue = basevalue < ab? basevalue:ab;
                 beta = beta<basevalue?beta:basevalue;
                 if(beta<=alpha) break;
               }
    }
    board[x][y]=0;
    return ismax? playervalue:basevalue;
}
    

int main(int, char** argv) {
    std::ifstream fin(argv[1]);
    std::ofstream fout(argv[2]); 
    initTable();   
    read_board(fin);
    nextmove(fout);  
    fin.close();
    fout.close();
    return 0;
}


int calculatevalue(bool isMax){
   int sum=0;
    bool flag[15][15][4] ={0};
    vector<int> baselineP(10,0);
    vector<int> playerP(10,0);
    int pthreat=0,bthreat=0;
    for(int i=0;i<SIZE;i++){
        for(int j=0;j<SIZE;j++){
          if(board[i][j]!=0){
              int doubly3=0,dead4=0;
              bool cocomax = board[i][j]==player;
              int cnt=1,locked=0, left=1,right=1;
              //-----------------------------------------------------------------------------------
              if(!flag[i][j][0])
              {flag[i][j][0]=1;
              while(j-left>=0 && board[i][j-left] == board[i][j]){
                  flag[i][j-left][0]=1;
                  cnt++;
                  left++;
              } if(j-left>=0 && board[i][j-left] == 3-board[i][j]) locked++;
              while(j+right<=SIZE-1 && board[i][j+right] == board[i][j]){
                  flag[i][j+right][0]=1;
                  cnt++;
                  right++;
              } if(j+right<=SIZE-1  && board[i][j+right] == 3-board[i][j]) locked++;

              if(cnt >=5){
                 if(cocomax) playerP[9]++;
                 else baselineP[9] ++;
              } else if ((cnt==4 && locked==0)){
                  if(cocomax) playerP[8]++, pthreat+=2;
                 else baselineP[8] ++,bthreat+=2;
              }  else if ((cnt==4 && locked==1)){
                  dead4++;
                  if(cocomax) playerP[7]++,bthreat+=1;
                 else baselineP[7] ++,pthreat+=1;
              }else if ((cnt==3 && locked==0)){
                 doubly3++;
                 if(cocomax) playerP[6]++;
                 else baselineP[6] ++;
              } else if ((cnt==3 && locked==1)){
                 if(cocomax) playerP[5]++;
                 else baselineP[5] ++;
              }else if(cnt==2 && locked==0) {
                  if(cocomax) playerP[4]++;
                  else baselineP[4]++;
              }else if(cnt==2 && locked==1){
                  if(cocomax) playerP[3]++;
                  else baselineP[3]++;
              } else if ((cnt==1 && locked==0)){
                 if(cocomax) playerP[2]++;
                 else baselineP[2] ++;
              } else if ((cnt==1 && locked==1)){
                 if(cocomax) playerP[1]++;
                 else baselineP[1] ++;
              }}
              //----------------------------------------------------------------------------------- 
             cnt=1,locked=0,left=1,right=1;
             if(!flag[i][j][1])
              { flag[i][j][1]=1;
                while(i-left>=0 && j-left>=0 && board[i-left][j-left] == board[i][j]){
                  flag[i-left][j-left][1]=1;
                  cnt++;
                  left++;
              } if(i-left>=0 && j-left>=0  && board[i-left][j -left] == 3-board[i][j]) locked++;
              while(i+right<=SIZE-1 && j+right<=SIZE -1&& board[i+right][j+right] == board[i][j]){
                flag[i+right][j+right][1]=1;
                  cnt++;
                  right++;
              } if(i+right<=SIZE-1  && j+right<=SIZE -1 &&board[i+right][j+right] == 3-board[i][j]) locked++;

            if(cnt >=5){
                 if(cocomax) playerP[9]++;
                 else baselineP[9] ++;
              } else if ((cnt==4 && locked==0)){
                  if(cocomax) playerP[8]++, pthreat+=2;
                 else baselineP[8] ++,bthreat+=2;
              }  else if ((cnt==4 && locked==1)){
                  dead4++;
                  if(cocomax) playerP[7]++,bthreat+=1;
                 else baselineP[7] ++,pthreat+=1;
              }else if ((cnt==3 && locked==0)){
                 doubly3++;
                 if(cocomax) playerP[6]++;
                 else baselineP[6] ++;
              } else if ((cnt==3 && locked==1)){
                 if(cocomax) playerP[5]++;
                 else baselineP[5] ++;
              }else if(cnt==2 && locked==0) {
                  if(cocomax) playerP[4]++;
                  else baselineP[4]++;
              }else if(cnt==2 && locked==1){
                  if(cocomax) playerP[3]++;
                  else baselineP[3]++;
              } else if ((cnt==1 && locked==0)){
                 if(cocomax) playerP[2]++;
                 else baselineP[2] ++;
              } else if ((cnt==1 && locked==1)){
                 if(cocomax) playerP[1]++;
                 else baselineP[1] ++;
              }}
            //-----------------------------------------------------------------------------------
              cnt=1,locked=0,left=1,right=1;
              if(!flag[i][j][2])
              { flag[i][j][2]=1;
                while(i-left>=0 && board[i-left][j] == board[i][j]){
                  flag[i-left][j][2]=1;
                  cnt++;
                  left++;
              } if(i-left>=0 && board[i-left][j] == 3-board[i][j]) locked++;
              while(i+right<=SIZE-1 && board[i+right][j] == board[i][j]){
                  flag[i+right][j][2]=1;
                  cnt++;
                  right++;
              } if(i+right<=SIZE-1  && board[i+right][j] == 3-board[i][j]) locked++;

              if(cnt >=5){
                 if(cocomax) playerP[9]++;
                 else baselineP[9] ++;
              } else if ((cnt==4 && locked==0)){
                  if(cocomax) playerP[8]++, pthreat+=2;
                 else baselineP[8] ++,bthreat+=2;
              }  else if ((cnt==4 && locked==1)){
                dead4++;
                  if(cocomax) playerP[7]++,bthreat+=1;
                 else baselineP[7] ++,pthreat+=1;
              }else if ((cnt==3 && locked==0)){
                 doubly3++;
                 if(cocomax) playerP[6]++;
                 else baselineP[6] ++;
              } else if ((cnt==3 && locked==1)){
                 if(cocomax) playerP[5]++;
                 else baselineP[5] ++;
              }else if(cnt==2 && locked==0) {
                  if(cocomax) playerP[4]++;
                  else baselineP[4]++;
              }else if(cnt==2 && locked==1){
                  if(cocomax) playerP[3]++;
                  else baselineP[3]++;
              } else if ((cnt==1 && locked==0)){
                 if(cocomax) playerP[2]++;
                 else baselineP[2] ++;
              } else if ((cnt==1 && locked==1)){
                 if(cocomax) playerP[1]++;
                 else baselineP[1] ++;
              }}
           
              cnt=1,locked=0,left=1,right=1;
              if(!flag[i][j][3])
              { flag[i][j][3]=1;
                while(i-left>=0 && j+left<=SIZE-1 && board[i-left][j+left] == board[i][j]){
                    flag[i-left][j+left][3]=1;
                  cnt++;
                  left++;
              } if(i-left>=0 && j+left<=SIZE-1  && board[i-left][j +left] == 3-board[i][j]) locked++;
              while(i+right<=SIZE-1 && j-right>=0&& board[i+right][j-right] == board[i][j]){
                  flag[i+right][j-right][3]=1;
                  cnt++;
                  right++;
              } if(i+right<=SIZE-1  && j-right>=0 &&board[i+right][j-right] == 3-board[i][j]) locked++;

              if(cnt >=5){
                 if(cocomax) playerP[9]++;
                 else baselineP[9] ++;
              } else if ((cnt==4 && locked==0)){
                  if(cocomax) playerP[8]++, pthreat+=2;
                 else baselineP[8] ++,bthreat+=2;
              }  else if ((cnt==4 && locked==1)){
                  dead4++;
                  if(cocomax) playerP[7]++,bthreat+=1;
                 else baselineP[7] ++,pthreat+=1;
              }else if ((cnt==3 && locked==0)){
                 doubly3++;
                 if(cocomax) playerP[6]++;
                 else baselineP[6] ++;
              } else if ((cnt==3 && locked==1)){
                 if(cocomax) playerP[5]++;
                 else baselineP[5] ++;
              }else if(cnt==2 && locked==0) {
                  if(cocomax) playerP[4]++;
                  else baselineP[4]++;
              }else if(cnt==2 && locked==1){
                  if(cocomax) playerP[3]++;
                  else baselineP[3]++;
              } else if ((cnt==1 && locked==0)){
                 if(cocomax) playerP[2]++;
                 else baselineP[2] ++;
              } else if ((cnt==1 && locked==1)){
                 if(cocomax) playerP[1]++;
                 else baselineP[1] ++;
              }}
              if(doubly3>=2){
                if(cocomax) {
                    playerP[6]-=2;
                    playerP[8]++;}
                else {
                    baselineP[6]-=2;
                    baselineP[8]++;
                }
              }
              else if(doubly3 && dead4){
                if(cocomax){
                    playerP[6]--,playerP[7]--;
                    playerP[8]++;
                }else{
                    baselineP[6]--,baselineP[7]--;
                    baselineP[8]++;
                }
              }
 
                    
          }   
        }
    }  
    
    if(playerP[9]) return INT_MAX;
    if(baselineP[9]) return INT_MIN;
    //if(baselineP[8]) return INT_MIN;
    //if(playerP[8]) return INT_MAX;

   // int x=1;
    for(int i=3;i<9;i++){
        sum += (playerP[i]*pvalue[i-1] - baselineP[i]*bvalue[i-1]);
    }
    return sum; 
}
void calculate(){
     bool flag[15][15][4] ={0};
    vector<int> baselineP(10,0);
    vector<int> playerP(10,0);
    bool p_speciail_threat=false;
    bool b_speciail_threat=false;
    int pthreat=0,bthreat=0;
    for(int i=0;i<SIZE;i++){
        for(int j=0;j<SIZE;j++){
          if(board[i][j]!=0){
              int doubly3=0,dead_4=0;
              bool cocomax = board[i][j]==player;
              int cnt=1,locked=0, left=1,right=1;
              //-----------------------------------------------------------------------------------
              if(!flag[i][j][0])
              {flag[i][j][0]=1;
              while(j-left>=0 && board[i][j-left] == board[i][j]){
                  flag[i][j-left][0]=1;
                  cnt++;
                  left++;
              } if(j-left>=0 && board[i][j-left] == 3-board[i][j]) locked++;
              while(j+right<=SIZE-1 && board[i][j+right] == board[i][j]){
                  flag[i][j+right][0]=1;
                  cnt++;
                  right++;
              } if(j+right<=SIZE-1  && board[i][j+right] == 3-board[i][j]) locked++;
               if(cnt >=5){
                 if(cocomax) playerP[9]++;
                 else baselineP[9] ++;
                 statusboard[i][j][0]= cocomax?p_FIVE:b_FIVE;
                 
              } else if ((cnt==4 && locked==0)){
                  if(cocomax) playerP[8]++, pthreat+=2;
                 else baselineP[8] ++,bthreat+=2;
                 statusboard[i][j][0]= p_live_four;
              }  else if ((cnt==4 && locked==1)){
                  if(cocomax) playerP[7]++,bthreat+=1;
                 else baselineP[7] ++,pthreat+=1;
                 statusboard[i][j][0]=p_dead_four;
              }else if ((cnt==3 && locked==0)){
                 doubly3++;
                 if(cocomax) playerP[6]++;
                 else baselineP[6] ++;
                 statusboard[i][j][0]= p_live_three;
              } else if ((cnt==3 && locked==1)){
                 if(cocomax) playerP[5]++;
                 else baselineP[5] ++;
                 statusboard[i][j][0]=p_dead_three;
              }else if(cnt==2 && locked==0) {
                  if(cocomax) playerP[4]++;
                  else baselineP[4]++;
                  statusboard[i][j][0]=p_live_two;
              }else if(cnt==2 && locked==1){
                  if(cocomax) playerP[3]++;
                  else baselineP[3]++;
                  statusboard[i][j][0]=p_dead_two;
              } else if ((cnt==1 && locked==0)){
                 if(cocomax) playerP[2]++;
                 else baselineP[2] ++;
                 statusboard[i][j][0]= p_live_one;
              } else if ((cnt==1 && locked==1)){
                 if(cocomax) playerP[1]++;
                 else baselineP[1] ++;
                 statusboard[i][j][0]=p_dead_one;
              } else statusboard[i][j][0]=p_dead;
                statusboard[i][j+cnt-1][0] = statusboard[i][j][0];}
              //----------------------------------------------------------------------------------- 
             cnt=1,locked=0,left=1,right=1;
             if(!flag[i][j][1])
              { flag[i][j][1]=1;
                while(i-left>=0 && j-left>=0 && board[i-left][j-left] == board[i][j]){
                  flag[i-left][j-left][1]=1;
                  cnt++;
                  left++;
              } if(i-left>=0 && j-left>=0  && board[i-left][j -left] == 3-board[i][j]) locked++;
              while(i+right<=SIZE-1 && j+right<=SIZE -1&& board[i+right][j+right] == board[i][j]){
                flag[i+right][j+right][1]=1;
                  cnt++;
                  right++;
              } if(i+right<=SIZE-1  && j+right<=SIZE -1 &&board[i+right][j+right] == 3-board[i][j]) locked++;

             if(cnt >=5){
                 if(cocomax) playerP[9]++;
                 else baselineP[9] ++;
                 statusboard[i][j][1]=b_FIVE;
                 
              } else if ((cnt==4 && locked==0)){
                  if(cocomax) playerP[8]++, pthreat+=2;
                 else baselineP[8] ++,bthreat+=2;
                 statusboard[i][j][1]=b_live_four;
              }  else if ((cnt==4 && locked==1)){
                  if(cocomax) playerP[7]++,bthreat+=1;
                 else baselineP[7] ++,pthreat+=1;
                 statusboard[i][j][1]=b_dead_four;
              }else if ((cnt==3 && locked==0)){
                 doubly3++;
                 if(cocomax) playerP[6]++;
                 else baselineP[6] ++;
                 statusboard[i][j][1]=b_live_three;
              } else if ((cnt==3 && locked==1)){
                 if(cocomax) playerP[5]++;
                 else baselineP[5] ++;
                 statusboard[i][j][1]=b_dead_three;
              }else if(cnt==2 && locked==0) {
                  if(cocomax) playerP[4]++;
                  else baselineP[4]++;
                  statusboard[i][j][1]=b_live_two;
              }else if(cnt==2 && locked==1){
                  if(cocomax) playerP[3]++;
                  else baselineP[3]++;
                  statusboard[i][j][1]=b_dead_two;
              } else if ((cnt==1 && locked==0)){
                 if(cocomax) playerP[2]++;
                 else baselineP[2] ++;
                 statusboard[i][j][1]= b_live_one;
              } else if ((cnt==1 && locked==1)){
                 if(cocomax) playerP[1]++;
                 else baselineP[1] ++;
                 statusboard[i][j][1]=b_dead_one;
              } else statusboard[i][j][1]=b_dead;
                statusboard[i+cnt-1][j+cnt-1][1] = statusboard[i][j][1];}
            //-----------------------------------------------------------------------------------
              cnt=1,locked=0,left=1,right=1;
              if(!flag[i][j][2])
              { flag[i][j][2]=1;
                while(i-left>=0 && board[i-left][j] == board[i][j]){
                  flag[i-left][j][2]=1;
                  cnt++;
                  left++;
              } if(i-left>=0 && board[i-left][j] == 3-board[i][j]) locked++;
              while(i+right<=SIZE-1 && board[i+right][j] == board[i][j]){
                  flag[i+right][j][2]=1;
                  cnt++;
                  right++;
              } if(i+right<=SIZE-1  && board[i+right][j] == 3-board[i][j]) locked++;

               if(cnt >=5){
                 if(cocomax) playerP[9]++;
                 else baselineP[9] ++;
                 statusboard[i][j][2]= b_FIVE;
                 
              } else if ((cnt==4 && locked==0)){
                  if(cocomax) playerP[8]++, pthreat+=2;
                 else baselineP[8] ++,bthreat+=2;
                 statusboard[i][j][2]=b_live_four;
              }  else if ((cnt==4 && locked==1)){
                  if(cocomax) playerP[7]++,bthreat+=1;
                 else baselineP[7] ++,pthreat+=1;
                 statusboard[i][j][2]=b_dead_four;
              }else if ((cnt==3 && locked==0)){
                 doubly3++;
                 if(cocomax) playerP[6]++;
                 else baselineP[6] ++;
                 statusboard[i][j][2]=b_live_three;
              } else if ((cnt==3 && locked==1)){
                 if(cocomax) playerP[5]++;
                 else baselineP[5] ++;
                 statusboard[i][j][2]=b_dead_three;
              }else if(cnt==2 && locked==0) {
                  if(cocomax) playerP[4]++;
                  else baselineP[4]++;
                  statusboard[i][j][2]=b_live_two;
              }else if(cnt==2 && locked==1){
                  if(cocomax) playerP[3]++;
                  else baselineP[3]++;
                  statusboard[i][j][2]=b_dead_two;
              } else if ((cnt==1 && locked==0)){
                 if(cocomax) playerP[2]++;
                 else baselineP[2] ++;
                 statusboard[i][j][2]= b_live_one;
              } else if ((cnt==1 && locked==1)){
                 if(cocomax) playerP[1]++;
                 else baselineP[1] ++;
                 statusboard[i][j][2]=b_dead_one;
              } else statusboard[i][j][2]=b_dead;
                statusboard[i+cnt-1][j][2] = statusboard[i][j][2];}
           
              cnt=1,locked=0,left=1,right=1;
              if(!flag[i][j][3])
              { flag[i][j][3]=1;
                while(i-left>=0 && j+left<=SIZE-1 && board[i-left][j+left] == board[i][j]){
                    flag[i-left][j+left][3]=1;
                  cnt++;
                  left++;
              } if(i-left>=0 && j+left<=SIZE-1  && board[i-left][j +left] == 3-board[i][j]) locked++;
              while(i+right<=SIZE-1 && j-right>=0&& board[i+right][j-right] == board[i][j]){
                  flag[i+right][j-right][3]=1;
                  cnt++;
                  right++;
              } if(i+right<=SIZE-1  && j-right>=0 &&board[i+right][j-right] == 3-board[i][j]) locked++;

              if(cnt >=5){
                 if(cocomax) playerP[9]++;
                 else baselineP[9] ++;
                 statusboard[i][j][3]= b_FIVE;
                 
              } else if ((cnt==4 && locked==0)){
                  if(cocomax) playerP[8]++, pthreat+=2;
                 else baselineP[8] ++,bthreat+=2;
                 statusboard[i][j][3]=b_live_four;
              }  else if ((cnt==4 && locked==1)){
                  if(cocomax) playerP[7]++,bthreat+=1;
                 else baselineP[7] ++,pthreat+=1;
                 statusboard[i][j][3]=b_dead_four;
              }else if ((cnt==3 && locked==0)){
                 doubly3++;
                 if(cocomax) playerP[6]++;
                 else baselineP[6] ++;
                 statusboard[i][j][3]= b_live_three;
              } else if ((cnt==3 && locked==1)){
                 if(cocomax) playerP[5]++;
                 else baselineP[5] ++;
                 statusboard[i][j][3]= b_dead_three;
              }else if(cnt==2 && locked==0) {
                  if(cocomax) playerP[4]++;
                  else baselineP[4]++;
                  statusboard[i][j][3]= b_live_two;
              }else if(cnt==2 && locked==1){
                  if(cocomax) playerP[3]++;
                  else baselineP[3]++;
                  statusboard[i][j][3]=b_dead_two;
              } else if ((cnt==1 && locked==0)){
                 if(cocomax) playerP[2]++;
                 else baselineP[2] ++;
                 statusboard[i][j][3]=b_live_one;
              } else if ((cnt==1 && locked==1)){
                 if(cocomax) playerP[1]++;
                 else baselineP[1] ++;
                 statusboard[i][j][3]= b_dead_one;
              } else statusboard[i][j][3]=b_dead;
                statusboard[i+cnt-1][j-cnt+1][3] = statusboard[i][j][3];
              }                 
          }   
        }
    }  
}
void printstatus(){
    for(int i=0;i<15;i++){
        for(int j=0;j<15;j++)
        {
            for(int k=0;k<4;k++){
                if(statusboard[i][j][k]!=0) cout << i << " " << j << " " << k << "is" << st[statusboard[i][j][k]] << "\n";
            }
           
        } 
    }
}
int pv[12]= {0,25,125,100,500,400,1200,2000,6000,10000,10000};
int bv[10]= {0,50,250,200,1800,800,2400,15000,15000};
int cv[10]= {1,2,3,0,0,3,2,1};

// dead1=1 live1=2 dead2=3 live2=4 dead3=5 live3=6 dead4=7 live4=8 five=9  

int computepoint(int x, int y,bool ismax){
    int dir[8][2] =  {{-1,-1},{-1,0},{-1,1},{0,-1},{0,1},{1,-1},{1,0},{1,1}};
    board[x][y] = ismax?player:3-player;
    int sum=0;
    for(int i=0;i<8;i++){
        int tmp = statusboard[x+dir[i][0]][y+dir[i][1]][cv[i]];
        if(0<=x+dir[i][0] && x+dir[i][0]<15 && 0<=y+dir[i][1] && y+dir[i][1]<15){
            if(board[x+dir[i][0]][y+dir[i][1]] == board[x][y]){
                 sum+=pv[tmp+2];
            } else sum+= bv[tmp];
        }
    }
    board[x][y]=0;
    return sum;

}
bool iswin(int x, int y){
    int cnt=1,left=1,right=1;
    while(y-left>=0 && board[x][y-left] == board[x][y]){
                  cnt++;
                  left++;
              } 
    while(y+right<=SIZE-1 && board[x][y+right] == board[x][y]){
                  cnt++;
                  right++;
              } 
     
    if(cnt>=5) return true;
    cnt=1,left=1,right=1;
    while(x-left>=0 && board[x-left][y] == board[x][y]){
                  cnt++;
                  left++;
              } 
    while(x+right<=SIZE-1 && board[x+right][y] == board[x][y]){
                  cnt++;
                  right++;
              } 
    if(cnt>=5) return true;
    cnt=1,left=1,right=1;
    while (x-left>=0 &&y-left >=0 && board[x-left][y-left] == board[x][y])
    {
        cnt++;
        left++;
    }
    while(x+right<SIZE && y+right<SIZE && board[x+right][y+right] == board[x][y]){
        cnt++;
        right++; 
    }
    if(cnt>=5) return true;
    cnt=1,left=1,right=1;
     while (x-left>=0 &&y+left<SIZE && board[x-left][y+left] == board[x][y])
    {
        cnt++;
        left++;
    }
    while(x+right<SIZE && y-right>=0 && board[x+right][y-right] == board[x][y]){
        cnt++;
        right++; 
    }
    if(cnt>=5) return true;
    else return false;
    
}

bool isconnect(int x, int y){
    bool coco = false;
    if(board[x][y]) return coco;
    int dic[16][2] = {{-1,-1},{-1,0},{-1,1},{0,-1},{0,1},{1,1},{1,-1},{1,0}};
    for(int i=0;i<8;i++){
        if (x+dic[i][0] >=0 && y+dic[i][1]>=0 && x+dic[i][0] <= SIZE-1 && y + dic[i][1] <= SIZE-1){
			coco = coco || (board[x+dic[i][0]][y+dic[i][1]]);
		}
    }
    return coco;

}

