#include <iostream>
#include <fstream>
#include <vector>
#include <time.h>
#include <map>
#include <limits.h>
#include <algorithm>
#include <stdio.h>

#define time_limit SHRT_MAX
using namespace std;

ofstream Output_C("result_close.txt");
ofstream Output_P("result_temp.txt");
ofstream Output("result.txt");
const double min_support=0.01;
const int largest_num=95;
const double total_customer=600.00;
double time_max=-1;
double threshold;
map <int,int> close_result_map;
map <int,int> frequency_to_close_result_map;

//FILE* output_anwer;
typedef struct
{
      int item_number;
      int occurrence_number;
      int time;
      char symbol;
}item_set;

typedef struct
{
      int sequence_number;
      int index_number;
}index_set;

typedef struct
{
      int item_number;
	  int support;
	  bool same_time;
	  char symbol;
	  bool appearance;
}count_set;

vector<vector<item_set> > database;
// vector<vector<item_set> > answer;
vector<vector<vector<item_set> > >close_result;
//**************************************************************************************************************
//		incision_strategy
//**************************************************************************************************************

void incision_strategy(int* count_1_item,item_set** database_temp)
{//對於每個使用者做time分析和計算min_support 換使用者就將整理好的 time放入database_temp中
     ifstream Input_TP("test600(only1)");
     int record[4]={0};
     int previous_sid=-1;
     bool* customer_counted = new bool [largest_num];//單個sequence計算是否有出現
     int* item_sequence=new int[largest_num];//計算每個sequence長度
     int item_number=0;
     vector<item_set>::iterator it;
     vector<item_set>::iterator min;

     for(int i=0;i<largest_num;i++)
         count_1_item[i]=0;

     vector<item_set>  time_cout[time_limit];

     while(Input_TP.eof()==false)
     {
          bool cin_success=1;
          for(int i=0; i<4; i++)
          {
              if(!(Input_TP >> record[i]))
                    cin_success=0;
          }
          if(cin_success==0)
               break;
          if(record[3]>time_max)
               time_max=record[3];
    //      cout << record[0]<<" "<< record[1]<<" "<< record[2]<<" "<< record[3]<< endl;
          if(record[0]!=previous_sid) //different  cumstomer => store data and initial!!!
          {
              // system("PAUSE");
               if(previous_sid!=-1)                               //sort into database_temp
			   {
                   database_temp[previous_sid]= new item_set[item_number*2+1];
                   database_temp[previous_sid][0].item_number=item_number*2; //第0個放置長度
                   int j=1; //第0個放置長度
                   for(int i=0;i<=time_max;i++)                     //sort and transfer to
                       while(time_cout[i].size()!=0)
                       {
                              min= time_cout[i].begin();
                              for ( it=time_cout[i].begin() ; it < time_cout[i].end(); it++)
                              {
                                  if(it->item_number < min->item_number)
                                        min=it;
                              }
                              database_temp[previous_sid][j]=*min;
                              j++;
                              time_cout[i].erase(min);
                       }
			   }
			   item_number=0;
               previous_sid=record[0];
               for(int i=0;i<largest_num;i++) //initial value
               {
                   customer_counted[i]=0;
                   item_sequence[i]=0;
               }
               for(int i=0;i<=time_max;i++)
               {
                   time_cout[i].clear();
               }
          }

          item_number++;
          if(customer_counted[record[1]]==0)    //increase support
          {    count_1_item[record[1]]++;
               customer_counted[record[1]]++;
          }
          item_set temp_head={record[1],item_sequence[record[1]],record[2],'+'};
          item_set temn_tail={record[1],item_sequence[record[1]],record[3],'-'};

          time_cout[record[2]].push_back(temp_head);
          time_cout[record[3]].push_back(temn_tail);
          item_sequence[record[1]]++;

     }
	 Input_TP.close();
     delete [] customer_counted;
     delete [] item_sequence;
                                   //the last user put into array
	{
    database_temp[previous_sid]= new item_set[item_number*2+1];
    database_temp[previous_sid][0].item_number=item_number*2; //第0個放置長度
    int j=1; //第0個放置長度
    for(int i=0;i<=time_max;i++)                     //sort and transfer to
         while(time_cout[i].size()!=0)
         {
               min= time_cout[i].begin();
               for ( it=time_cout[i].begin() ; it < time_cout[i].end(); it++)
               {
                    if(it->item_number < min->item_number)
                    min=it;
               }
               database_temp[previous_sid][j]=*min;
               j++;
               time_cout[i].erase(min);
         }
    }

}

//**************************************************************************************************************
//			infrequent_elimination
//**************************************************************************************************************


void infrequent_elimination(int* count_1_item,item_set** database_temp)
{
	    for(int i=1; i<=int(total_customer);i++)
		{
			vector<item_set> one_sequence_temp;
		    for(int j=1;j<=database_temp[i][0].item_number;j++)
			{
				if(count_1_item[ database_temp[i][j].item_number ] > threshold) //將要計算放入
					one_sequence_temp.push_back(database_temp[i][j]);
			}
			if(one_sequence_temp.size()!=0)
				database.push_back(one_sequence_temp);
		}
	//	ofstream Output("result.txt");
      //  cout<<(int)database.capacity()<<" "<<(int)database.size()<<endl;

        for(int i=0;i<database.size();i++)
        {
            Output<<"sequence "<<i<<endl;
           for(int j=0;j<database.at(i).size();j++)
               Output<< database.at(i).at(j).item_number<<" "<<database.at(i).at(j).occurrence_number<<" "<<database.at(i).at(j).symbol<<" "<<database.at(i).at(j).time<<endl ;
            Output<<database[i].size()<<endl;
        }
       Output.close();
}

//**************************************************************************************************************
//			prefix_scan
//**************************************************************************************************************


void prefix_scan( vector<item_set> partial_answer,vector<index_set> index)
{

	int* count_item_temp= new int[largest_num*4]; // 計算單sequence是否出現  前largest_num 為'+'
	vector<count_set> count_item;		 		  //計算總出現次數		     後largest_num   '-'
																//	         在下面是 同時間的

//**************************************************************************************************************
//			取得最長同時間的位置
//**************************************************************************************************************

	vector<item_set>::reverse_iterator rit;
	vector<item_set>::reverse_iterator same_time_sequence_begin;//取得最長同時間位置
	rit=partial_answer.rbegin();		//從後面往前推
	same_time_sequence_begin=partial_answer.rbegin();
	int last_same_time=rit->time;	//取得時間點
	for(++rit;rit < partial_answer.rend();++rit)     //取得最長同時間的位置
	{
		if(rit->time == last_same_time)
			same_time_sequence_begin = rit;
		else
			break;
	}


//**************************************************************************************************************
//			cout support
//**************************************************************************************************************


    for(int i=0;i<largest_num*4;i++) //initial
        count_item_temp[i]=-1;

	for(int z=0;z<index.size();z++) // cout support
	{
		int i = index[z].sequence_number;
		for(int j=0;j<count_item.size();j++) //清空暫存
		{
			count_item[j].appearance=0;
		}
		int lat_time=database[i][ index[z].index_number ].time;  //上一個解答的時間
		for(int j=index[z].index_number+1;j<database[i].size();)
		{
			if(database[i][j].time==lat_time)      //字串前頭與上個解答同時間的判斷
			{
				int positiveORnegative;
				if(database[i][j].symbol=='+')
					positiveORnegative = largest_num*2;
				else
					positiveORnegative = largest_num*3;
				if(count_item_temp[database[i][j].item_number + positiveORnegative ]==-1)
				{
					count_item_temp[database[i][j].item_number + positiveORnegative ] = count_item.size();
					count_set count_set_temp;
					count_set_temp.item_number = database[i][j].item_number;
					count_set_temp.support = 1;
					count_set_temp.same_time = 1;
					if(database[i][j].symbol=='+')
						count_set_temp.symbol = '+';
					else
						count_set_temp.symbol = '-';
					count_set_temp.appearance = 1;
					count_item.push_back(count_set_temp);
				}
				else
				{
					if (count_item[ count_item_temp[database[i][j].item_number + positiveORnegative ] ].appearance==0)
					{
						count_item[ count_item_temp[database[i][j].item_number + positiveORnegative ] ].appearance =1;
						count_item[ count_item_temp[database[i][j].item_number + positiveORnegative ] ].support++;
					}
				}
				j++;
			}
			else
			{
				rit=same_time_sequence_begin;
				for(int time_temp = database[i][j].time;j<database[i].size();j++)
				{
					if(database[i][j].time != time_temp)        //時間不同跳出for loop
                        break;

					int positiveORnegative;
					if(database[i][j].symbol=='+')
						positiveORnegative = 0;
					else
						positiveORnegative = largest_num;
					if(count_item_temp[database[i][j].item_number + positiveORnegative ]==-1)
					{
						count_item_temp[database[i][j].item_number + positiveORnegative ] = count_item.size();
						count_set count_set_temp;
						count_set_temp.item_number = database[i][j].item_number;
						count_set_temp.support = 1;
						count_set_temp.same_time = 0;
						if(database[i][j].symbol=='+')
							count_set_temp.symbol = '+';
						else
							count_set_temp.symbol = '-';
						count_set_temp.appearance = 1;
						count_item.push_back(count_set_temp);
					}
					else
					{
						if (count_item[ count_item_temp[database[i][j].item_number + positiveORnegative ] ].appearance==0)
						{
							count_item[ count_item_temp[database[i][j].item_number + positiveORnegative ] ].appearance =1;
							count_item[ count_item_temp[database[i][j].item_number + positiveORnegative ] ].support++;
						}
					}

					if(rit < partial_answer.rbegin())		//代表前面與解答同時間串一樣
					{
						int positiveORnegative;
						if(database[i][j].symbol=='+')
							positiveORnegative = largest_num*2;
						else
							positiveORnegative = largest_num*3;
						if(count_item_temp[database[i][j].item_number + positiveORnegative ]==-1)
						{
							count_item_temp[database[i][j].item_number + positiveORnegative ] = count_item.size();
							count_set count_set_temp;
							count_set_temp.item_number = database[i][j].item_number;
							count_set_temp.support = 1;
							count_set_temp.same_time = 1;
							if(database[i][j].symbol=='+')
								count_set_temp.symbol = '+';
							else
								count_set_temp.symbol = '-';
							count_set_temp.appearance = 1;
							count_item.push_back(count_set_temp);
						}
						else
						{
							if (count_item[ count_item_temp[database[i][j].item_number + positiveORnegative ] ].appearance==0)
							{
								count_item[ count_item_temp[database[i][j].item_number + positiveORnegative ] ].appearance =1;
								count_item[ count_item_temp[database[i][j].item_number + positiveORnegative ] ].support++;
							}
						}
					}
					else if(rit->item_number == database[i][j].item_number &&rit >= partial_answer.rbegin())		//做比較
						rit--;
				}
			}
		}//cou support one sequence finished
	}//cout support finished
	delete [] count_item_temp;
    //cout<< " prefix_scan count end "<<endl;

   // ofstream prefix_scan_count("prefix_scan_count");
//    for(int i=1;i<largest_num*4;i++)
//        cout<<i<<"  "<<i%largest_num << "  "<< count_item[i].support<<"  "<<count_item[i].symbol<<endl;
/*
    for(int i=1;i<1493;i++) {
		if(count_item[i].support>threshold)
			cout<<i<<"	"<<count_item[i].support<<endl;
	}
*/
//    cout<<"1492	"<<1492%1000<<"	"<<count_item[1492].support<<"	"<<count_item[1492].symbol<<endl;
//    cout<<"1493	"<<1493%1000<<"	"<<count_item[1493].support<<"	"<<count_item[1493].symbol<<endl;

//    system("pause");
    //bool z=0;


//**************************************************************************************************************
//			append answer
//**************************************************************************************************************
//	cout<<count_item.size()<<endl;
//	system("pause");
	for(int i=1;i<count_item.size();i++)			//進行接上動作 和 OUTPUT
		if(count_item[i].support > threshold)
		{
        //    z=1;
       //     cout<<"in connect"<<endl;
        //    cout<<endl<<"partial_answer.size():  "<< partial_answer.size()<<endl;

          //  cout<<endl << "the index now to search " <<endl;

       /*     for(int i=0;i<database.size();i++) // cout support
            {
                cout << index[i]<<endl;
            }*/

			item_set item_set_temp ;
			item_set_temp.item_number= count_item[i].item_number ;

			if(count_item[i].same_time==0)
				item_set_temp.time = partial_answer.back().time +1;
			else
				item_set_temp.time = partial_answer.back().time;

			if(count_item[i].symbol=='+')
				item_set_temp.symbol='+';
			else
				item_set_temp.symbol='-';

			int sequence_item_positive=0;
      //      cout<<endl<<"item_number '"<<item_set_temp.item_number<<"' time '"<<item_set_temp.time<<"' symbol "<<item_set_temp.symbol<<endl;
        //    cout<< " prefix_scan to build item_set_temp & find same occurrence_number +  start "<<endl;

//**************************************************************************************************************
//			prunning
//**************************************************************************************************************

			if(item_set_temp.symbol=='-')
			{				   //尋找是否有 + 號在解答內
				sequence_item_positive=-1;
				for(rit=partial_answer.rbegin();rit < partial_answer.rend();++rit)
				{
					if(item_set_temp.item_number == rit -> item_number )
					{
						if(rit->symbol == '+')  //如果先找到同個的'+' 存 occurrence_number 若先找到 '-' 為缺乏 同個 '+'
							sequence_item_positive= rit-> occurrence_number;
						break;
					}
				}
			}
		/*	else
			{
				for(rit=partial_answer.rbegin();rit < partial_answer.rend();++rit)
				{
					if(item_set_temp.item_number == rit -> item_number )
					{
						if(rit->symbol == '+')  //same item_value but the last one is not mach by '-'
							sequence_item_positive= -1;
						break;
					}
				}
			}*/
			//cout<<"sequence_item_positive "<<sequence_item_positive<<endl;
			if(sequence_item_positive==-1)		//缺乏 同樣的 + 在前 直接跳過 PRUNNING
				continue;

        //   cout<< " prefix_scan to build item_set_temp & find same occurrence_number +  end "<<endl;

//**************************************************************************************************************
//			find new index
//**************************************************************************************************************

			vector<index_set> index_new ;   //新的 INDEX
			index_set index_set_temp;
			int min_occurrence_number=INT_MAX;
			bool broken=0;
			//ofstream prefix_scan_index("prefix_scan_index");
			for(int z=0; z<index.size() && broken != 1;z++)
			{
				int j=index[z].sequence_number;
				index_set_temp.index_number = database[j].size();

                int k;
				if(count_item[i].same_time==0)  //not same time
				{
					for(k=index[z].index_number+1;k < database[j].size() && broken != 1;k++)
					{
						if(database[j][k].item_number == item_set_temp.item_number && database[j][k].symbol == item_set_temp.symbol)
						{
							index_set_temp.index_number = k;
							index_set_temp.sequence_number=j;
							break;
						}
					}
				}
				else   //same time
				{
					int lat_time=database[j][ index[z].index_number ].time;  //上一個解答的時間
					k=index[z].index_number+1;
					for(bool find=0;k<database[j].size()&&find==0;)
					{
						if(database[j][k].time==lat_time&&database[j][k].item_number == item_set_temp.item_number && database[j][k].symbol == item_set_temp.symbol)      //字串前頭與上個解答同時間的判斷
						{
							index_set_temp.index_number=k;
							index_set_temp.sequence_number=j;
							find=1;
							break;
						}
						rit=same_time_sequence_begin;
						for(int time_temp = database[j][k].time;k<database[j].size();k++)
						{
							if(database[j][k].time != time_temp)        //時間不同跳出for loop
								break;

							if(rit < partial_answer.rbegin()&&database[j][k].item_number == item_set_temp.item_number && database[j][k].symbol == item_set_temp.symbol)		//代表前面與解答同時間串一樣
							{
								index_set_temp.index_number=k;
								index_set_temp.sequence_number=j;
								find=1;
								break;
							}
							else if(rit->item_number == database[j][k].item_number &&rit >= partial_answer.rbegin())		//做比較
								rit--;
						}
					}
				}
                if(k<database[j].size())
                {//cout<<k<<endl;
                //    cout<< "found the index "<<j <<"  "<<k<<endl;
                    if(item_set_temp.symbol=='-' && database[j][k].occurrence_number > sequence_item_positive)
                    {		//處理不同 occurence 的問題
                    //    cout<<" in if different occurence "<< endl;
                        rit=partial_answer.rbegin();
                        int l;
                        for(l=k-1;l>=0 && database[j][l].item_number!= item_set_temp.item_number;l--)
                        { // 往前搜尋是否有正確的PATTERN
                            if(database[j][l].item_number == rit -> item_number && database[j][l].symbol == rit -> symbol &&rit -> item_number!=item_set_temp.item_number)
                                rit++;
                        }

                        if(rit -> item_number!=item_set_temp.item_number)
                        {
                            count_item[i].support-- ; //代表此occurence為錯誤 刪除support
                            index_set_temp.index_number= database[j].size();
                            if(count_item[i].support < threshold)
                            {
                                broken = 1;   //錯誤
                                continue;
                            }
                        }
                    }
                    if(database[j][k].occurrence_number < min_occurrence_number)
                        min_occurrence_number=database[j][k].occurrence_number;

					index_new.push_back(index_set_temp);
					//	cout<< j<<" "<<database[j][k].item_number<<" "<<k<<endl;
				}
				 //single sequence index search finished
				 // cout<<endl<<"sequence  item_number  index "<<endl;
			//	cout << j<< " "<<item_set_temp.item_number<<" "<< index_new[j] <<endl;
			} // seqeunce index search finished
			//cout<< "broken  "<<broken<<endl;
			if(broken == 1)
			{
                continue;
            //    system("pause");
			}

			item_set_temp.occurrence_number = min_occurrence_number;

			partial_answer.push_back(item_set_temp);

			vector<item_set>::iterator it;

//**************************************************************************************************************
//			output answer
//**************************************************************************************************************

			int positive=0;
			int negative=0;
		//	cout<< " prefix_scan partiall answer "<<endl;
			for(it=partial_answer.begin();it<partial_answer.end();it++)
			{
				//cout<<it->symbol<<endl;
				if(it->symbol == '+')
					positive++;
				else
					negative++;

			//	cout<<"prefix_scan "<<it->item_number<<" "<< it->occurrence_number<<" "<<it->time<<" "<<it->symbol<<endl;
			//	cout<<"positive "<<positive <<"  negative "<<negative<<endl;
			}


            //system("pause");
			if(positive==negative)		//正負一樣多代表 答案沒有缺陷
			{
			   // cout<<endl<<"answer"<<endl;
				//fprintf(output_anwer,"(");
				Output_P<<"(";
				int time_temp=partial_answer.front().time;
				for(it=partial_answer.begin();it<partial_answer.end();it++)
				{
					if(time_temp!=it->time)
                        Output_P<<"),(";
					//	fprintf(output_anwer,") (");
					Output_P<<it->item_number<<it->symbol<<"*";
					//fprintf(output_anwer," %d%c",it->item_number<<it->symbol);
					time_temp= it->time;
				}
				//fprintf(output_anwer,")\n");
				Output_P <<"):"<<count_item[i].support<<endl;

				bool in_close_set=false;
				bool in_close_set2=false;
				//cout<<"close_result.size()" <<close_result.size()<<endl;
				if(frequency_to_close_result_map.count(count_item[i].support)==0)
				{
					frequency_to_close_result_map[count_item[i].support]=close_result.size();
					vector<vector<item_set> > tempset;
					close_result.push_back(tempset);
				}
				int close_index = frequency_to_close_result_map[count_item[i].support] ;
				close_result_map[close_index] = count_item[i].support;
				for (int z =0;z<close_result[close_index].size();z++)
				{
					//	cout<<"count_item[i].support"<<count_item[i].support<<" close_result_map[z]"<<close_result_map[z]<<endl;
						int time_temp1=partial_answer.front().time;
						int time_temp2=close_result[close_index][z][0].time;
						int index_close = 0;
						for(it=partial_answer.begin();it<partial_answer.end();it++)	//
						{
						//	cout<<"判斷 close_result 是否在  partial_answer 的 sub set內"<<endl;
							if(it->item_number== close_result[close_index][z][index_close].item_number&&it->symbol==close_result[close_index][z][index_close].symbol
								&&((time_temp1!=it->time&&time_temp2!=close_result[close_index][z][index_close].time)||(time_temp1==it->time&&time_temp2==close_result[close_index][z][index_close].time)))
							{
								//cout<<"in same"<<endl;
								index_close ++;										//an item have found
								time_temp2 = close_result[close_index][z][index_close].time;
							}
							time_temp1= it->time;
							if(index_close>=close_result[close_index][z].size())	//all item have found
							{
								cout<<"close_resul.size()"<<close_result[close_index][z].size()<<" index_close"<<index_close<<" count_item[i].support"<<count_item[i].support<<endl;
								int time_temp=partial_answer.front().time;
								for(it=partial_answer.begin();it<partial_answer.end();it++)
								{
									if(time_temp!=it->time)
										cout<<"),(";
									//	fprintf(output_anwer,") (");
									cout<<it->item_number<<it->symbol<<"*";
									//fprintf(output_anwer," %d%c",it->item_number<<it->symbol);
									time_temp= it->time;
								}
								//fprintf(output_anwer,")\n");
								cout <<"):"<<count_item[i].support<<endl;
								cout<<"all item have found 1"<<endl;
								in_close_set=true;
								break;
							}
						}
						if(in_close_set==true) //close_result 在 partial_answer的 close set 中	刪除 close_result此解
						{
							cout<<"close_result 在 partial_answer的 close set 中	刪除 close_result此解				"<<endl;
							close_result[close_index].erase(close_result[close_index].begin()+z);
							close_result[close_index].insert(close_result[close_index].begin()+z,partial_answer);
							break;
						}

						time_temp1=partial_answer.front().time;
						time_temp2=close_result[close_index][z][0].time;
						it=partial_answer.begin();
						for(index_close = 0;index_close<close_result[close_index][z].size();index_close ++)  //
						{
						//	cout<<"判斷 partial_answer 是否在  close_result 的 sub set內"<<endl;
							if(it->item_number== close_result[close_index][z][index_close].item_number&&it->symbol==close_result[close_index][z][index_close].symbol
								&&((time_temp1!=it->time&&time_temp2!=close_result[close_index][z][index_close].time)||(time_temp1==it->time&&time_temp2==close_result[close_index][z][index_close].time)))
							{
							//	cout<<"in same2"<<endl;
								it++;												//an item have found
								time_temp1= it->time;
							}
							time_temp2 = close_result[close_index][z][index_close].time;
							if(it>=partial_answer.end())	//all item have found
							{
							//	cout<<"all item have found 2"<<endl;
								in_close_set2=true;
								break;
							}
						}

					if(in_close_set2== true)
						break;
				}
				if(in_close_set2==false&&in_close_set==false)
				{
				//	cout<<"push_back(partial_answer)"<<endl;
					close_result[close_index].push_back(partial_answer);

				}
			}
			if(positive==negative)		//正負一樣多代表 答案沒有缺陷 印出要求格式
			{
				vector<item_set> partial_answer_output;
				for(int y=0;y<partial_answer.size();y++)
				{
					partial_answer_output.push_back(partial_answer[y]);
				}
			    //cout<<endl<<"answer"<<endl;
				//fprintf(output_anwer,"(");
				Output<<"(";
				int time_temp=partial_answer_output.front().time;
				int small_mouse=0;
				int change_time=0;
				for(it=partial_answer_output.begin();it<partial_answer_output.end();it++)
				{

					vector<item_set>::iterator next_time;
					if(time_temp!=it->time&&!((small_mouse==2)&&(change_time==it->time)))
					{
						Output<<")(";
					}
					time_temp= it->time;
					bool negative_has=0;
					for(next_time=it;next_time<partial_answer_output.end();next_time++)
					{
						if(next_time->time!=it->time)
							break;
						if(next_time->symbol=='-')
						{
						//	fprintf(output_anwer,") (");
							Output<<next_time->item_number<<next_time->symbol<<"*";
							partial_answer_output.erase(next_time);
							next_time--;
							negative_has=1;
							continue;
						}
					}
					if(time_temp!=it->time)
					{
						it--;
						small_mouse=0;
						continue;
					}
					if(it->symbol=='+')
					{
						if(negative_has==1||((small_mouse==2)&&(change_time==it->time)))
						{
							Output<<")@(";
							small_mouse=0;
						}
						int time_temp2=next_time->time;
						bool got_mix=0;
						for(;next_time<partial_answer_output.end();next_time++)
						{
							if(time_temp2!=next_time->time)
							{
								break;
							}
							if(next_time->item_number==it->item_number &&next_time->symbol=='-')
							{
								Output<<it->item_number<<"*";
								change_time=next_time->time;
								partial_answer_output.erase(next_time);
								got_mix=1;
								next_time--;
								small_mouse=2;

								continue;
							}
						}
						if(got_mix==0)
							Output<<it->item_number<<it->symbol<<"*";
					}

					//fprintf(output_anwer," %d%c",it->item_number<<it->symbol);
				}
				//fprintf(output_anwer,")\n");
				Output <<"):"<<count_item[i].support<<endl;
			}

			//system("pause");
			prefix_scan(partial_answer,index_new);
			partial_answer.pop_back();

		}
	/*	if(z==0)
		{
		cout<<endl<<"the partial answer"<<endl;
			vector<item_set>::iterator it;

				cout<<"(";
				int time_temp=partial_answer.front().time;
				for(it=partial_answer.begin();it<partial_answer.end();it++)
				{
					if(time_temp!=it->time)
						cout<<") (";
					cout<<" "<<it->item_number<<it->symbol;
					time_temp= it->time;
				}
        cout<<endl<<" no other item more than threshold "<<endl;
		}*/

}


//**************************************************************************************************************
//			first_prefix_scan
//**************************************************************************************************************


void first_prefix_scan(int* count_1_item)
{
		for(int i=1;i<largest_num;i++)
			if(count_1_item[i] > threshold)
			{
				vector<item_set> partial_answer;
				vector<index_set> index ;
				item_set item_set_temp ;
				item_set_temp.item_number=i;
				item_set_temp.occurrence_number=0;
				item_set_temp.time=0;
				item_set_temp.symbol='+';

				index_set index_set_temp;
				//ofstream first_prefix_scan_index("first_prefix_scan_index");
			//	cout<<endl<<"first_prefix_scan_index"<<endl;
				for(int j=0; j<database.size();j++)
					{
						for(int k=0;k<database.at(j).size();k++)
						{
							if(database[j][k].item_number == i)
							{
								index_set_temp.sequence_number = j;
								index_set_temp.index_number = k;
								index.push_back(index_set_temp);
								break;
							}
						}
					//	cout<< j<<" "<<i<<" "<<index[j]<<endl;
					}

				partial_answer.push_back(item_set_temp);

			//	system("pause");
			//	cout<< "first_prefix_scan end & prefix_scan start"<<endl;
			//	cout<<endl<<"item_number "<< item_set_temp.item_number<<" symbol "<<item_set_temp.symbol<<endl;
				prefix_scan(partial_answer,index);
			}
}


//**************************************************************************************************************
//			main
//**************************************************************************************************************


int main(int argc, char* argv[])
{

//        min_support=atof(argv[1]);
//        largest_num=atoi(argv[2]);   //item的總項數
//        total_customer=atoi(argv[3]);
        threshold=total_customer*min_support;

        cout << "CTMiner starts:" << endl;
        time_t start,end;
        start=clock();

        int* count_1_item= new int[largest_num]; //計算第一次總出現次數
        item_set** database_temp = new item_set*[int(total_customer)+1];

        incision_strategy(count_1_item,database_temp);  //轉換和計算次數

        ofstream first_count ("first_count");
        for(int i=0;i<largest_num;i++)
           first_count<<i<<" "<<count_1_item[i]<<endl;

		infrequent_elimination(count_1_item,database_temp);

        for(int i=0;i<=total_customer;i++)
           delete[] database_temp[i];
        delete [] database_temp;

	//	FILE* output_anwer = fopen("answer.txt","a");
		//cout<< "infrequent_elimination end and first_prefix_scan start"<<endl;
		//system("pause");
    	first_prefix_scan(count_1_item);


		for(int k=0;k<close_result.size();k++)
		for(int i=0;i<close_result[k].size();i++)
		{
			Output_C<<"(";		//output close set
			int time_temp=close_result[k][i][0].time;
			for(int j=0;j<close_result[k][i].size();j++)
			{
				if(time_temp!=close_result[k][i][j].time)
					Output_C<<"),(";
				//	fprintf(output_anwer,") (");
				Output_C<<close_result[k][i][j].item_number<<close_result[k][i][j].symbol<<"*";
				//fprintf(output_anwer," %d%c",it->item_number<<it->symbol);
				time_temp= close_result[k][i][j].time;
			}
					//fprintf(output_anwer,")\n");
			Output_C <<"):"<<close_result_map[k]<<endl;
		}


        end=clock();
        cout << endl << "CTMiner finished & Time Spend: "<<(end-start)/1000<<" secs " << (end-start)%1000 << " minsecs.";
		Output<< endl << "CTMiner finished & Time Spend: "<<(end-start)/1000<<" secs " << (end-start)%1000 << " minsecs.";
		delete [] count_1_item;
     //   fclose(output_anwer);

        return 0;
}

