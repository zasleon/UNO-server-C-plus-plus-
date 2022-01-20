#ifndef INCLUDE_UNO_AI_H
#define INCLUDE_UNO_AI_H

#include "UNO_deck_process.h"

UNO_player* UNO_check_who_is_the_next(UNO_room* this_room,UNO_player* p1)//确认谁是下家
{
	int pointer;
	
	if(this_room->clock_direct)//确认下家
	{
		pointer=p1->room_member_NO+1;
		if(pointer==UNO_member_limit)pointer=0;
		UNO_player* next_player=&this_room->player[pointer];
		while(next_player->state==UNO_empty)
		{
			if(this_room->game_over)return NULL;//如果游戏结束了，不用继续找了
			pointer++;
			if(pointer==UNO_member_limit)pointer=0;
			next_player=&this_room->player[pointer];
		}

	}
	else
	{
		pointer=p1->room_member_NO-1;
		if(pointer==0)pointer=UNO_member_limit-1;
		UNO_player* next_player=&this_room->player[pointer];
		while(next_player->state==UNO_empty)
		{
			if(this_room->game_over)return NULL;//如果游戏结束了，不用继续找了
			pointer--;
			if(pointer==-1)pointer=UNO_member_limit-1;
			next_player=&this_room->player[pointer];
		}
	}
	return &this_room->player[pointer];
}

void UNO_AI_which_color_i_wanna_play(UNO_room* this_room,UNO_player* p1)//分析自己手牌，确定自己想出什么颜色牌，如果自己遇到黑牌选择情况，选择那个颜色
{
	cout<<"AI思考要什么颜色\n";
	int color_box[4];//4种颜色，存储颜色卡数量，因为颜色序号为1，2，3，4，所以存到0，1，2，3
		for(int k=0;k<4;k++)
		{
			color_box[k]=0;
		}
		int i=0;
		while(i<UNO_deck_card_limit&&p1->card_color[i]!=UNO_none)//若当前卡槽不为空 且 没到头
		{
			if(p1->card_color[i]!=UNO_black)//如果不为黑色，则记录自己手中x颜色卡的数量+1
			{
				color_box[p1->card_color[i]-1]++;//该颜色数量+1
			}
			i++;//看下一个卡槽
		}

		int color_number=0;//最大数量初始值为0
		
		p1->i_want_play_this_card=UNO_none;//清空原先想要的颜色
		for(i=0;i<4;i++)
			if(color_box[i]>color_number)//如果当前颜色数量大于最大数量，
			{
				color_number=color_box[i];//更新最大数量
				bool find_another=false;
				for(int j=0;j<UNO_member_limit;j++)
				{
					if(this_room->player[j].card_color[1]==UNO_none&&this_room->player[j].only_color==i+1)//如果当前选的颜色会让某人赢，换别的颜色
					{
						find_another=true;
						break;
					}
				}
				if(find_another)continue;
				p1->i_want_play_this_card=i+1;//将当前想出的颜色修正为当前颜色
			}

		if(p1->i_want_play_this_card==UNO_none)//如果没获取到想要的颜色，说明可能自己选的颜色会导致别人赢
		{
			cout<<"AI暂时没找到我要的颜色\n";
			for(int k=0;k<4;k++)
			{
				color_box[k]=0;
			}
			for(int i=0;i<UNO_member_limit;i++)
				if(this_room->player[i].card_color[1]==UNO_none&&this_room->player[i].only_color!=UNO_none)
				{
					color_box[this_room->player[i].only_color-1]++;
				}
			for(int i=0;i<4;i++)
				if(color_box[i]==0)
				{
					p1->i_want_play_this_card=i+1;//选别人没有的颜色，让大家都不得不抽卡
					cout<<"破釜沉舟"<<i+1<<endl;
					return;
				}
		}
}

bool UNO_prior_use_forbidden_or_reverse_or_add_2(UNO_room* this_room,UNO_player* p1)//优先打出禁止/逆转/+2的卡
{
	UNO_player* next_player=UNO_check_who_is_the_next(this_room,p1);//确认下家
	if(next_player==NULL)
	{
		if(!this_room->game_over)
			cout<<"怎么可能没结束游戏就返回空值?\n";
		return true;
	}
	if(this_room->current_running_effect==UNO_none)//如果当前没有加罚效果
		if(next_player->card_color[3]==UNO_none)//如果下家手牌少于4张
			for(int i=0;i<UNO_deck_card_limit;i++)
			{
				if(this_room->current_color==p1->card_color[i]&&(p1->card_effect[i]==UNO_forbid||p1->card_effect[i]==UNO_reverse||p1->card_effect[i]==UNO_ADD_2))
				{
					UNO_play_this_card(this_room,p1,i);//打出同色的功能牌
					return true;
				}
				if(this_room->last_effect==p1->card_effect[i]&&(p1->card_effect[i]==UNO_forbid||p1->card_effect[i]==UNO_reverse||p1->card_effect[i]==UNO_ADD_2))
				{
					UNO_play_this_card(this_room,p1,i);//打出相同效果的功能牌
					return true;
				}
			}

	return false;
		
}

bool UNO_AI_think_whether_use_black_card(UNO_room* this_room,UNO_player* p1)//思考是否使用主动进攻使用黑牌
{
	int black_card_number=0;
	int normal_card_number=0;
	for(int i=0;i<UNO_deck_card_limit;i++)//统计手里黑牌和其他牌的数量
	{
		if(p1->card_color[i]==UNO_none)break;
		if(p1->card_color[i]==UNO_black)
			black_card_number++;
		else
			normal_card_number++;
	}
	if(this_room->current_running_effect==UNO_none)//如果当前没有加罚效果
		if(black_card_number!=0&&black_card_number>=normal_card_number)//如果黑牌数量比其他牌多或齐平，开始优先使用黑牌
		{
			UNO_AI_which_color_i_wanna_play(this_room,p1);//确认要打哪种颜色
			for(int i=7;i>=0;i--)//卡组黑牌一共就8张，一般黑色万能牌排在黑色+4后面，优先使用万能牌
				if(p1->card_color[i]==UNO_black)
				{
					UNO_play_this_card(this_room,p1,i);
					return true;
				}
		}
	return false;
}

bool UNO_AI_think_whether_use_add_4_to_next_player(UNO_room* this_room,UNO_player* p1)//下家只剩一张手卡了，使用黑色+4
{
	UNO_player* next_player=UNO_check_who_is_the_next(this_room,p1);//确认下家
	if(next_player==NULL)
	{
		if(!this_room->game_over)
			cout<<"怎么可能没结束游戏就返回空值?\n";
		return true;
	}
	if(next_player->card_color[1]==UNO_none)//如果他只剩一张卡了
	{
		for(int i=0;i<UNO_deck_card_limit;i++)
			if(p1->card_effect[i]==UNO_ADD_4)//打出黑+4
			{
				UNO_play_this_card(this_room,p1,i);
				return true;
			}
			else
				break;
	}

	return false;
}

bool UNO_AI_think_whether_change_color(UNO_room* this_room,UNO_player* p1)//思考是否在紧急情况用黑卡改变颜色
{
	for(int i=0;i<UNO_member_limit;i++)
		if(this_room->player[i].card_color[1]==UNO_none&&this_room->current_color==this_room->player[i].only_color)//如果场上有人只有一张卡，且其拥有手卡的可能颜色与当前颜色一致
		{
			for(int j=7;j>=0;j--)
				if(p1->card_color[j]==UNO_black)//尝试打出黑卡修改颜色，优先使用万能牌
				{
					UNO_play_this_card(this_room,p1,j);
					return true;
				}
		}
	return false;
}


bool UNO_AI_careful_follow_play_card(UNO_room* this_room,UNO_player* p1)//谨慎跟牌
{
		for(int i=0;i<UNO_deck_card_limit;i++)
		{
			if(p1->card_color[i]==UNO_none)//到头了还没有颜色匹配的
			{
				break;
			}

			//节省黑卡
			if(p1->card_color[i]==UNO_black)continue;

			//如果有加罚效果
			//if(this_room->current_running_effect==UNO_add_card)

			//如果当前没有加罚效果且颜色相同 或 效果相同
			if((this_room->current_running_effect!=UNO_add_card&&p1->card_color[i]==this_room->current_color)||p1->card_effect[i]==this_room->last_effect)//有匹配的
			{
				for(int j=0;j<UNO_member_limit;j++)
				{
					if(this_room->player[j].card_color[1]==UNO_none&&this_room->player[j].only_color==p1->card_color[i])//如果场上有人只有一张牌且当前要出的牌将是和那个人同色，过
						continue;
				}

				UNO_play_this_card(this_room,p1,i);//把那张牌打出,打出牌函数内自带回合切换
				return true;//作出行动了
			}
		
		}
	return false;
}



bool UNO_AI_simple_follow_play_card(UNO_room* this_room,UNO_player* p1)//单纯跟牌
{
		for(int i=0;i<UNO_deck_card_limit;i++)
		{
			if(p1->card_color[i]==UNO_none)//到头了还没有颜色匹配的
			{
				break;
			}

			//如果有加罚效果
			//if(this_room->current_running_effect==UNO_add_card)

			//如果当前没有加罚效果且颜色相同 或 效果相同
			if((this_room->current_running_effect!=UNO_add_card&&p1->card_color[i]==this_room->current_color)||p1->card_effect[i]==this_room->last_effect)//有匹配的
			{
				cout<<this_room->whose_turn<<"号AI计算完了\n";
				
				UNO_play_this_card(this_room,p1,i);//把那张牌打出,打出牌函数内自带回合切换
				return true;//作出行动了
			}
		
		}
	return false;
}

void UNO_AI_think(UNO_player* p1)
{
	UNO_room* this_room=&uno_room[p1->room_No];
	//等候准备指令下来，开局所有工作准备完了则开始思考，否则进行等候
	while(!this_room->game_start)//游戏还没准备好开始,则进行等待
		Sleep(100);

	bool done;//当前是否完成自己回合的一次操作
	while(true)
	{
		done=false;//重置为否
		while(&this_room->player[this_room->whose_turn]!=p1)//如果当前不是我的回合
		{
			if(this_room->game_over)//如果当前没在游戏中了，结束AI思考线程
			{
				cout<<"AI"<<p1->room_member_NO<<"exit\n";
				return;
			}
			//如果能“抢”，“抢”

			Sleep(UNO_AI_wait_time);
		}

		for(int i=0;i<25;i++)
		{
			Sleep(UNO_AI_think_time/25);//让其他人有“抢”的时间
			if(this_room->game_over)//如果当前没在游戏中了，结束AI思考线程
			{
				cout<<"AI"<<p1->room_member_NO<<"exit\n";
				return;
			}
		}

		

		if(&this_room->player[this_room->whose_turn]!=p1)//如果当前不是我的回合了，被“抢”了，则继续等
		{
			continue;
		}
		
		//挂起其他所有人线程
		cout<<this_room->whose_turn<<"号AI开始计算\n";


		//分析自己手牌，确定自己想出什么颜色牌，如果之后自己遇到黑牌选择情况，选择那个颜色
		UNO_AI_which_color_i_wanna_play(this_room,p1);


		//（函数返回false）表示没打出牌，（函数返回true）表示如果之前那些决策导致机器人已经打出手卡，continue表示进行下一个等待循环，等待下一个自己的回合到来
		
		




		//如果自己下家只剩一张牌且确定only_color和last_color一样，则打出万能牌，没有的话尝试打出+2，没有的话尝试打出+4，没有的话打出功能牌forbid，没有的话打出功能牌reverse
	
		//如果下家只剩一张卡，打出+4
		if(UNO_AI_think_whether_use_add_4_to_next_player(this_room,p1))continue;
		
		//如果场上有人只剩一张牌且确定only_color和last_color一样，则打出万能牌改变颜色，选择一个自己有的颜色但不是那人的only_color，或者随机改变颜色
		if(UNO_AI_think_whether_change_color(this_room,p1))continue;
		

		//如果自己下家手卡<=3张,尝试优先使用功能卡
		if(UNO_prior_use_forbidden_or_reverse_or_add_2(this_room,p1))continue;
		
		//如果自己只手卡<=3张且都是同色，打出黑卡变为自己另一张卡的颜色

		//优先出0

		//优先普通跟牌
		if(UNO_AI_careful_follow_play_card(this_room,p1))continue;//如果里面作出跟牌行动，进行下一个等待循环，等待下一个自己的回合到来
		
		//如果没有颜色和效果能跟，则考虑要不要出黑牌让自己尽可能获得主动权
		if(UNO_AI_think_whether_use_black_card(this_room,p1))continue;
		
		//执行到这里，说明之前没牌跟，进行抽卡
		UNO_user_apply_get_card(this_room,p1,1);
		Sleep(300);
		//抽卡后，如果有牌能出，出；如果没牌可出，选择“过”
		if(UNO_AI_careful_follow_play_card(this_room,p1))continue;//如果里面作出跟牌行动，进行下一个等待循环，等待下一个自己的回合到来
		//抽卡后，看自己要不要出黑卡，因为可能抽到黑卡
		if(UNO_AI_think_whether_use_black_card(this_room,p1))continue;
		//到这里说明即便抽卡后也没同色或同号牌可出，选择“过”
		UNO_user_apply_change_turn(this_room,p1);//机器人操作完成，切换角色回合，如果有加罚效果，在该函数内执行加罚

	}


	
	
}

#endif