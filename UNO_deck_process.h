#ifndef INCLUDE_UNO_deck_process_H
#define INCLUDE_UNO_deck_process_H

#include "UNO_room_process.h"

void UNO_lock_player(UNO_room* this_room,UNO_player* p1)//封锁房间其他用户线程，一般在成功能打出一张牌时使用
{
	if(this_room->whether_lock)return;//如果已经锁了，不执行再锁
	this_room->whether_lock=true;
	for(int i=0;i<UNO_member_limit;i++)
	{
		switch(this_room->player[i].state)
		{
			case UNO_human:
				if(this_room->player[i].room_member_NO!=p1->room_member_NO)//如果当前位子上人并非发起事件者
					SuspendThread(hthread[this_room->player[i].c1->member_No-1]);//锁住他的监听线程
				break;
			case UNO_empty:
				break;
			default://机器人
				if(this_room->player[i].room_member_NO!=p1->room_member_NO)//如果当前位子上人并非发起事件者
					SuspendThread(this_room->AI_thread[i]);//锁住他的监听线程
		}
	}
	Sleep(200);
}
void UNO_release_player(UNO_room* this_room,UNO_player* p1)//解放房间其他用户线程
{
	if(!this_room->whether_lock)return;//如果没锁，不执行解锁
	this_room->whether_lock=false;

	for(int i=0;i<UNO_member_limit;i++)
	{
		switch(this_room->player[i].state)
		{
			case UNO_human:
				if(this_room->player[i].room_member_NO!=p1->room_member_NO)//如果当前位子上人并非发起事件者
					ResumeThread(hthread[this_room->player[i].c1->member_No-1]);//解放他的监听线程
				
				break;
			case UNO_empty:
				break;
			default://机器人
				if(this_room->player[i].room_member_NO!=p1->room_member_NO)//如果当前位子上人并非发起事件者
					ResumeThread(this_room->AI_thread[i]);//解放他的监听线程
		}
	}
}
void UNO_clean_deck(UNO_room* this_room)//卡组全部置空
{
	for(int i=0;i<UNO_deck_card_limit;i++)
	{
		this_room->deck_card_color[i]=UNO_none;
		this_room->deck_card_effect[i]=UNO_none;
		this_room->tomb_card_color[i]=UNO_none;
		this_room->tomb_card_color[i]=UNO_none;
	}
}
void UNO_add_card_to_tomb(UNO_room* this_room,int color,int effect )//将该卡放入墓地
{
	int pointer=0;
	while(pointer<UNO_deck_card_limit)
	{
		if(this_room->tomb_card_color[pointer]!=UNO_none)
			pointer++;
		else
		{
			this_room->tomb_card_color[pointer]=color;
			this_room->tomb_card_effect[pointer]=effect;
			return;
		}
	}
}
void UNO_add_card_to_deck(UNO_room* this_room,int color,int effect )//将该卡放入卡组
{
	int pointer=0;
	while(pointer<UNO_deck_card_limit)
	{
		if(this_room->deck_card_color[pointer]!=UNO_none)
			pointer++;
		else
		{
			this_room->deck_card_color[pointer]=color;
			this_room->deck_card_effect[pointer]=effect;
			return;
		}
	}
}

void UNO_deck_card_all_into_tomb(UNO_room* this_room)//把卡组里的卡全回到墓地
{
	for(int i=0;i<UNO_deck_card_limit;i++)
	{
		this_room->tomb_card_color[i]=this_room->deck_card_color[i];
		this_room->tomb_card_effect[i]=this_room->deck_card_effect[i];
		this_room->deck_card_color[i]=UNO_none;
		this_room->deck_card_effect[i]=UNO_none;
	}
}

char* UNO_get_player_name(char* name,UNO_player* p1)
{
	memset(name,0,sizeof(name));
	//请确保name被初始化过={0}
	if(p1!=NULL)
		switch(p1->state)
		{
			case UNO_empty:
				addtext(name,"空");
				break;
			case UNO_human:
				addtext(name,p1->room_member_NO+1);
				addtext(name,"号玩家 ");
				addtext(name,p1->c1->client_name);
				addtext(name," ");
				break;
			case UNO_hard_AI:
				addtext(name,"一般机器人");
				addtext(name,p1->room_member_NO+1);
				addtext(name,"号");
				addtext(name," ");
				break;
			case UNO_simple_AI:
				addtext(name,"简单机器人");
				addtext(name,p1->room_member_NO+1);
				addtext(name,"号");
				addtext(name," ");
				break;
			default:addtext(name,"奇怪东西");
		}
	else
		return "";
	return name;
}

void UNO_game_add_log(UNO_room* this_room,UNO_player* p1,char* StrBuf)//发送游戏消息日志
{
	//没喊UNO
	//用户选择颜色超时
	//用户选择打牌超时
	//选择“过”
	//选择摸牌
	//打出黑色牌后选择颜色(接收到颜色更新消息后客户端自动添加log)
	//禁止牌，谁被跳过了！
	//回合逆转
	//谁接受加罚
	//打出一张牌/翻牌
	//发动了“抢”
	JSON_package json_msg;
	json_msg.add_item("signal",UNO_add_game_log);
	json_msg.add_item("log_content",StrBuf);
	char name[200]={0};
	if(p1!=NULL)
		json_msg.add_item("event_name",UNO_get_player_name(name,p1));
	Sleep(send_sleep_time);
	for(int i=0;i<UNO_member_limit;i++)
		if(this_room->player[i].state==UNO_human)
			send_msg(this_room->player[i].c1,json_msg.to_StrBuf());
	Sleep(send_sleep_time);
}


int check_how_many_card_in_deck(UNO_room* this_room)//检查卡组多少卡
{
	int deck_card_number=0;
	while(this_room->deck_card_color[deck_card_number]!=UNO_none)
	{
		deck_card_number++;
		if(deck_card_number==UNO_deck_card_limit)//如果已经到达卡组尽头，说明卡组满的
			break;
	}
	return deck_card_number;
}
int check_how_many_card_in_tomb(UNO_room* this_room)//检查墓地多少卡
{
	int tomb_card_number=0;
	while(this_room->tomb_card_color[tomb_card_number]!=UNO_none)
	{
		tomb_card_number++;
		if(tomb_card_number==UNO_deck_card_limit)//如果已经到达卡组尽头，说明卡组满的
			break;
	}
	return tomb_card_number;
}
int check_how_many_card_in_player(UNO_player* p1)//检查玩家多少手卡
{
	int player_card_number=0;
	while(p1->card_color[player_card_number]!=UNO_none)
	{
		player_card_number++;
		if(player_card_number==UNO_deck_card_limit)//如果已经到达卡组尽头，说明手卡满的
		{
			cout<<"手卡满？？？\n";
			break;
		}
	}
	return player_card_number;
}

char* UNO_get_this_card_color(int color)
{
			switch(color)
			{
				case UNO_green:return "绿色";
				case UNO_red:return "红色";
				case UNO_blue:return "蓝色";
				case UNO_yellow:return "黄色";
				case UNO_black:return "黑色";
				case UNO_none:return "none";
				default:;cout<<"??wrong_color??"<<color<<",";
					return "??wrong_color??";
			}
}
char* UNO_get_this_card_effect(int effect)
{
			switch(effect)
			{
				case UNO_ADD_4:		return "+4";
				case UNO_reverse:	return "逆转牌";
				case UNO_forbid:	return "阻碍牌";
				case UNO_ADD_2:		return "+2";break;
				case UNO_universal:	return "万能牌";
				case 1:return "1";
				case 2:return "2";
				case 3:return "3";
				case 4:return "4";
				case 5:return "5";
				case 6:return "6";
				case 7:return "7";
				case 8:return "8";
				case 9:return "9";
				case 0:return "0";
				default:;cout<<"??wrong_effect??"<<effect<<endl;
					return "??wrong_effect??";
			}
}

void UNO_cout_this_card(int color,int effect)
{
	cout<<UNO_get_this_card_color(color)<<UNO_get_this_card_effect(effect)<<endl;
}
void UNO_show_deck(UNO_room* this_room)//展示当前房间卡组所有卡
{
	char content[200]={0};
	cout<<"卡组内卡:\n";
	for(int kk=0;kk<UNO_deck_card_limit;kk++)
	{
		UNO_cout_this_card(this_room->deck_card_color[kk],this_room->deck_card_effect[kk]);
		cout<<endl;
	}
}
void UNO_update_all_info_to_himself(UNO_room* this_room,UNO_player* p1)//更新最后出的一张牌
{
	if(p1->state!=UNO_human)return;
	JSON_package json_msg;
	json_msg.add_item("signal",UNO_refresh_all_data);
	json_msg.add_item("last_color",this_room->last_color);
	json_msg.add_item("last_effect",this_room->last_effect);
	json_msg.add_item("current_color",this_room->current_color);
	json_msg.add_item("whose_turn",this_room->whose_turn);//谁的回合
	if(this_room->clock_direct)//顺时针逆时针
		json_msg.add_item("clock_direct",UNO_clockwise);
	else
		json_msg.add_item("clock_direct",UNO_anti_clockwise);

	JSON_package card_number;
	for(int i=0;i<UNO_member_limit;i++)
		card_number.add_item_array("card_number",check_how_many_card_in_player(&this_room->player[i]));//计算手卡数量
	json_msg.add_struct("card_number",card_number.get_json());
	//cout<<"检查结果:"<<check_how_many_card_in_player(p1)<<endl;

	//自己手卡所有内容
	JSON_package card_color;
	JSON_package card_effect;		
	for(int j=0;j<UNO_deck_card_limit;j++)//
		if(p1->card_color[j]!=UNO_none)
		{
			card_color.add_item_array("my_card_color",p1->card_color[j]);
			card_effect.add_item_array("my_card_effect",p1->card_effect[j]);
		}
		else
			break;
	if(p1->card_color[0]!=UNO_none)//掉线者会将所有牌放回墓地，从而上面计算没有手卡
	{
		json_msg.add_struct("my_card_color",card_color.get_json());//将自己手卡信息全加入结构体
		json_msg.add_struct("my_card_effect",card_effect.get_json());
	}
	send_msg(p1->c1,json_msg.to_StrBuf());
}


void UNO_broadcast_all_info(UNO_room* this_room)//全员刷新信息，切换回合后使用，某人非加罚抽卡后使用
{
	for(int i=0;i<UNO_member_limit;i++)
		if(this_room->player[i].state==UNO_human)
			UNO_update_all_info_to_himself(this_room,&this_room->player[i]);//更新所有信息	
}


void UNO_sys_change_turn(UNO_room* this_room)//下一个人的回合了，若该角色是机器人，执行AI思考
{
	//加罚与其他效果都在执行效果后的该函数内清空
	for(int i=0;i<UNO_member_limit;i++)
		this_room->player[i].whether_get_card=false;//所有人摸牌状态变为“没摸过”
	
	if(this_room->clock_direct)//顺时针
	{
		while(true)
		{
			this_room->whose_turn++;
			if(this_room->whose_turn==UNO_member_limit)
					this_room->whose_turn=0;
			while(this_room->player[this_room->whose_turn].state==UNO_empty)//若当前位置上为空，下一个
			{
				this_room->whose_turn++;
				if(this_room->whose_turn==UNO_member_limit)
					this_room->whose_turn=0;
			}
			//执行到这里必然找到不为空的位置者
			if(this_room->current_running_effect==UNO_forbid)//如果有禁止效果，跳过该玩家回合,继续寻找下个玩家并设置为他的回合
			{
				UNO_game_add_log(this_room,&this_room->player[this_room->whose_turn],"被跳过了!");
				this_room->current_running_effect=UNO_none;//清空禁止效果
			}
			else//如果没有禁止效果，破除死循环
				break;
		
		}
	}
	else
	{
		while(true)
		{
			this_room->whose_turn--;
			if(this_room->whose_turn<0)
					this_room->whose_turn=UNO_member_limit-1;
			while(this_room->player[this_room->whose_turn].state==UNO_empty)//若当前位置上为空，下一个
			{
				this_room->whose_turn--;
				if(this_room->whose_turn<0)
					this_room->whose_turn=UNO_member_limit-1;
			}
			//执行到这里必然找到不为空的位置者
			if(this_room->current_running_effect==UNO_forbid)//如果有禁止效果，跳过该玩家回合,继续寻找下个玩家并设置为他的回合
			{
				UNO_game_add_log(this_room,&this_room->player[this_room->whose_turn],"被跳过了!");
				this_room->current_running_effect=UNO_none;//清空禁止效果
			}
			else//如果没有禁止效果，破除死循环
				break;
		}
	}
	UNO_broadcast_all_info(this_room);
	
}



void UNO_refresh_player_card_add(UNO_player* p1,bool whether_refresh_data)//因为添加了手卡而更新他手卡，重新排序
{
	for(int i=0;i<UNO_deck_card_limit;i++)
	{
		if(p1->card_color[i]==UNO_none)
			break;
		for(int j=0;j<UNO_deck_card_limit;j++)//把颜色优先级最低的、序号最前面的往前排
		{
			if(p1->card_color[j]==UNO_none)
				break;

			if(p1->card_color[i]>p1->card_color[j])//如果颜色不同，优先级低的会往前排
			{
				int temp_color=p1->card_color[i];
				int temp_effect=p1->card_effect[i];
				p1->card_color[i]=p1->card_color[j];
				p1->card_effect[i]=p1->card_effect[j];
				p1->card_color[j]=temp_color;
				p1->card_effect[j]=temp_effect;
			}
			if(p1->card_color[i]==p1->card_color[j]&&p1->card_effect[i]>p1->card_effect[j])//如果颜色相同，序号小的会往前排
			{
				int temp_effect=p1->card_effect[i];
				p1->card_effect[i]=p1->card_effect[j];
				p1->card_effect[j]=temp_effect;
			}
		}
	}
	//如果是没UNO加罚，要更新数据
	//如果说是加罚，不用更新数据，加罚后会自动回合切换，在回合切换那里执行更新数据
	//如果说是首次所有人抽卡，则在翻牌时更新所有人数据，首次所有人抽卡相当于加罚
	//如果是自己选择摸牌，则更新数据所有人数据
	if(whether_refresh_data)
		UNO_broadcast_all_info(&uno_room[p1->room_No]);
	
}

void UNO_refresh_player_card_used(UNO_player* p1)//因为使用了手卡而更新他手卡，仅将后面的牌提上来
{
	int i;
	for(i=0;i<UNO_deck_card_limit;i++)
	{
		if(p1->card_color[i]==UNO_none)
			break;
	}
	if(i==UNO_deck_card_limit-1||p1->card_color[i+1]==UNO_none)//刚好用掉排序中的最后一张卡,不用排序
	{
		//用手牌后自动切换回合，在那里进行更新全员数据
		return;
	}

	while(true)//将后面那个卡排到前面来
	{
		p1->card_color[i]=p1->card_color[i+1];
		p1->card_effect[i]=p1->card_effect[i+1];
		i++;
		if(i==UNO_deck_card_limit-1||p1->card_color[i+1]==UNO_none)//如果说手卡槽到头了，或当前槽有卡而下个槽无卡，在之前操作已经复制到前面那个槽了，所以该槽置空并结束排序
		{
			p1->card_color[i]=UNO_none;
			p1->card_effect[i]=UNO_none;
			break;
		}
			
	}
	//用手牌后会自动切换回合，在那里进行更新通讯所有人手卡
}


void UNO_shuffle(UNO_room* this_room)//洗牌，【当抽卡后判断没卡后】、【初始化卡组后】使用该函数，将tomb墓地里的牌放入deck
{
	UNO_game_add_log(this_room,NULL,"系统开始洗牌");
	int card_number=check_how_many_card_in_tomb(this_room);//确认墓地里的卡片数量
	
	if(check_how_many_card_in_deck(this_room)>0)
	{
		//cout<<"卡组里卡不为0，不用洗牌！\n";
		return;
	}
	
	if(card_number==0)//墓地里卡为0，无法洗牌
	{
		//cout<<"墓地里卡为0，不能洗牌！\n";
		return;
	}

	int tomb_card_number=card_number;//确认完了卡片数量
	int result=0;
	while(true)
	{
		srand((unsigned)time(NULL)*rand()); //随机取值0-card_number-1
		
		result=rand()%(card_number-1-0);
		
		while(this_room->tomb_card_color[result]==UNO_none)//如果为空，看上一个卡槽里有没有牌
		{
			result--;
			if(result==-1)
				result=card_number-1;
		}//确认完牌了，将牌放入卡组
		UNO_add_card_to_deck(this_room,this_room->tomb_card_color[result],this_room->tomb_card_effect[result]);
		this_room->tomb_card_color[result]=UNO_none;this_room->tomb_card_effect[result]=UNO_none;//该卡槽置空
		tomb_card_number--;
		if(tomb_card_number==0)break;//若墓地里没卡了，不用洗了
	}

}




void UNO_ini_deck(UNO_room* this_room)//初始化卡组，只在游戏开始时用到，先将卡都放入墓地，再洗牌方式返回卡组
{
	//所有角色手中卡清空在start_game中使用

	//卡组墓地卡清空
	UNO_clean_deck(this_room);

	//往墓地里塞所有卡片
	//0都单张
	UNO_add_card_to_tomb(this_room,UNO_green,0);
	UNO_add_card_to_tomb(this_room,UNO_red,0);
	UNO_add_card_to_tomb(this_room,UNO_blue,0);
	UNO_add_card_to_tomb(this_room,UNO_yellow,0);

	//1-9，每个两张
	for(int i=1;i<10;i++)
	{
		UNO_add_card_to_tomb(this_room,UNO_green,i);
		UNO_add_card_to_tomb(this_room,UNO_green,i);
		UNO_add_card_to_tomb(this_room,UNO_red,i);
		UNO_add_card_to_tomb(this_room,UNO_red,i);
		UNO_add_card_to_tomb(this_room,UNO_blue,i);
		UNO_add_card_to_tomb(this_room,UNO_blue,i);
		UNO_add_card_to_tomb(this_room,UNO_yellow,i);
		UNO_add_card_to_tomb(this_room,UNO_yellow,i);
	}
	
	//+2，每个颜色两张
	UNO_add_card_to_tomb(this_room,UNO_green,UNO_ADD_2);
	UNO_add_card_to_tomb(this_room,UNO_green,UNO_ADD_2);
	UNO_add_card_to_tomb(this_room,UNO_red,UNO_ADD_2);
	UNO_add_card_to_tomb(this_room,UNO_red,UNO_ADD_2);
	UNO_add_card_to_tomb(this_room,UNO_blue,UNO_ADD_2);
	UNO_add_card_to_tomb(this_room,UNO_blue,UNO_ADD_2);
	UNO_add_card_to_tomb(this_room,UNO_yellow,UNO_ADD_2);
	UNO_add_card_to_tomb(this_room,UNO_yellow,UNO_ADD_2);

	//反转牌，每个两张
	UNO_add_card_to_tomb(this_room,UNO_green,UNO_reverse);
	UNO_add_card_to_tomb(this_room,UNO_green,UNO_reverse);
	UNO_add_card_to_tomb(this_room,UNO_red,UNO_reverse);
	UNO_add_card_to_tomb(this_room,UNO_red,UNO_reverse);
	UNO_add_card_to_tomb(this_room,UNO_blue,UNO_reverse);
	UNO_add_card_to_tomb(this_room,UNO_blue,UNO_reverse);
	UNO_add_card_to_tomb(this_room,UNO_yellow,UNO_reverse);
	UNO_add_card_to_tomb(this_room,UNO_yellow,UNO_reverse);

	//阻挡牌，每个两张
	UNO_add_card_to_tomb(this_room,UNO_green,UNO_forbid);
	UNO_add_card_to_tomb(this_room,UNO_green,UNO_forbid);
	UNO_add_card_to_tomb(this_room,UNO_red,UNO_forbid);
	UNO_add_card_to_tomb(this_room,UNO_red,UNO_forbid);
	UNO_add_card_to_tomb(this_room,UNO_blue,UNO_forbid);
	UNO_add_card_to_tomb(this_room,UNO_blue,UNO_forbid);
	UNO_add_card_to_tomb(this_room,UNO_yellow,UNO_forbid);
	UNO_add_card_to_tomb(this_room,UNO_yellow,UNO_forbid);

	//黑色万能牌4张
	UNO_add_card_to_tomb(this_room,UNO_black,UNO_universal);
	UNO_add_card_to_tomb(this_room,UNO_black,UNO_universal);
	UNO_add_card_to_tomb(this_room,UNO_black,UNO_universal);
	UNO_add_card_to_tomb(this_room,UNO_black,UNO_universal);

	//黑色+4，4张
	UNO_add_card_to_tomb(this_room,UNO_black,UNO_ADD_4);
	UNO_add_card_to_tomb(this_room,UNO_black,UNO_ADD_4);
	UNO_add_card_to_tomb(this_room,UNO_black,UNO_ADD_4);
	UNO_add_card_to_tomb(this_room,UNO_black,UNO_ADD_4);

	//洗牌，将墓地里所有牌洗到卡组里,洗三遍
	UNO_shuffle(this_room);
	UNO_deck_card_all_into_tomb(this_room);
	UNO_shuffle(this_room);
	UNO_deck_card_all_into_tomb(this_room);
	UNO_shuffle(this_room);
	
	//UNO_show_deck(this_room);//显示卡组里所有卡

}



void UNO_update_current_color(UNO_room* this_room,UNO_player* p1,int color)//告诉用户更新了可以出牌的颜色
{
	this_room->current_color=color;
	JSON_package json_msg;
	json_msg.add_item("signal",UNO_player_choose_color_change);
	json_msg.add_item("current_color",this_room->current_color);
	char name[200]={0};
	if(p1!=NULL)
		json_msg.add_item("event_name",UNO_get_player_name(name,p1));
	for(int i=0;i<UNO_member_limit;i++)//通知房间内所有活人更新当前可以出的牌的颜色
		if(this_room->player[i].state==UNO_human)
			send_msg(this_room->player[i].c1,json_msg.to_StrBuf());
}
void UNO_update_current_direct(UNO_room* this_room)//告诉用户更新了顺序方向，逆时针顺时针,在翻牌和打出这张牌时会使用
{
	if(this_room->clock_direct)
		this_room->clock_direct=false;
	else
		this_room->clock_direct=true;

	char log_content[300]={0};
	addtext(log_content,"当前顺序方向改变了!变为");
	if(this_room->clock_direct)
		addtext(log_content,"顺时针!");
	else
		addtext(log_content,"逆时针!");

	UNO_game_add_log(this_room,NULL,log_content);
}


void UNO_update_current_card(UNO_room* this_room,UNO_player* p1,int color,int effect)//更新最后出的一张牌
{
	this_room->last_color=color;
	this_room->last_effect=effect;
	if(this_room->last_color!=UNO_black)//如果出牌为黑牌，则在之前会让玩家主动变更颜色并赋予last_color，此处不予赋值，否则就将最后打出的一张牌的颜色设置为该颜色
		this_room->current_color=this_room->last_color;
	if(p1!=NULL)
	{
		char log_content[300]={0};
		addtext(log_content,"打出:");
		addtext(log_content,UNO_get_this_card_color(color));
		addtext(log_content,UNO_get_this_card_effect(effect));
		UNO_game_add_log(this_room,p1,log_content);//谁打出什么牌
		return;//在打出牌后执行的回合切换里执行全部更新
	}
	//执行到这里说明是首次翻牌

	UNO_broadcast_all_info(this_room);//更新所有信息
}




void UNO_declare_UNO(UNO_room* this_room,UNO_player* p1)
{
	if(p1->whether_declare_UNO)return;//已经声明过了
	if(p1->card_color[1]!=UNO_none)
		return;
	p1->whether_declare_UNO=true;
	UNO_game_add_log(this_room,p1,"UNO!他只剩最后一张手牌了!");
}


void UNO_game_over_process(UNO_room* this_room,UNO_player* p1,bool win)//p1赢了，或游戏人数不足了（只剩两个人游戏时，离开一个人后且不剩机器人时），处理游戏结束
{

	this_room->game_over=true;
	this_room->game_start=false;

	JSON_package json_msg;
	UNO_add_room_message(this_room,&json_msg);//添加房间成员信息

	for(int i=0;i<UNO_member_limit;i++)
	{
		if(this_room->player[i].state==UNO_human)
		{
			json_msg.add_item("your_number",i);
			this_room->player[i].c1->state=UNO_in_room;//状态变回在房间中
			if(win)//如果是获胜而结束的游戏
			{
				json_msg.add_item("winner_number",p1->room_member_NO);
				json_msg.add_item("signal",UNO_game_over_someone_win);//获胜
				char name[200]={0};
				json_msg.add_item("winner",UNO_get_player_name(name,p1));
				send_msg(this_room->player[i].c1,json_msg.to_StrBuf());				
			}
			else//如果是人数不足而结束的游戏
			{
				json_msg.add_item("signal",UNO_game_over_lack_player);
				send_msg(this_room->player[i].c1,json_msg.to_StrBuf());	
			}

		}
	}
}


bool UNO_judge_whether_can_play(UNO_room* this_room,UNO_player* p1,int choice)//判断是否能打出这张牌
{
	if(this_room->current_running_effect==UNO_add_card)//看看当前是否有加罚效果
		switch(this_room->last_effect)//查看最后一个人用的卡，能在+2后+4，但不能再+4后加2
		{
			case UNO_ADD_2://+2后面能加+4或+2
				if(p1->card_effect[choice]==UNO_ADD_4||p1->card_effect[choice]==UNO_ADD_2)
					return true;
				else
					return false;
			case UNO_ADD_4://+4后面只能+4
				if(p1->card_effect[choice]==UNO_ADD_4)
					return true;
				else
					return false;
		}

	//执行到这里，必然没施加加卡的效果
	
	if(p1->card_color[choice]==UNO_black)//黑牌可以直接出
		return true;

	if(p1->card_color[choice]!=this_room->current_color&&p1->card_effect[choice]!=this_room->last_effect)//如果选中的牌既不同色又不同号/同效果，不能打出
		return false;//无法使用

	return true;
}

void UNO_seize_turn(UNO_room* this_room,UNO_player* p1)//打出相同的牌
{
	if(this_room->whose_turn!=p1->room_member_NO)//如果当前不是我的回合
	{
		this_room->whose_turn=p1->room_member_NO;//强制变为我的回合	
		UNO_game_add_log(this_room,p1,"打出了相同的牌，抢占了该回合!");
	}
}

void UNO_player_choose_color(UNO_room* this_room,UNO_player* p1);
void UNO_play_this_card(UNO_room* this_room,UNO_player* p1,int choice)//打出这张牌
{

	if(choice<0||choice>UNO_deck_card_limit)//出牌指定位置越界，无视请求
	{
		//cout<<"出牌指定位置越界，无视请求\n";
		return;
	}
	if(p1->card_color[choice]==UNO_none)//该槽内没有卡
	{
		//cout<<choice<<"该槽内没有卡!\n";		
		UNO_update_all_info_to_himself(this_room,p1);//更新所有信息
		return;
	}

	//判定该牌能不能使用
	if(!UNO_judge_whether_can_play(this_room,p1,choice))//如果不能打出这张牌，进行提示
	{
		if(p1->state!=UNO_human){cout<<"机器人打出错误卡!";UNO_cout_this_card(p1->card_color[choice],p1->card_effect[choice]);return;}
		UNO_update_all_info_to_himself(this_room,p1);//更新所有信息
		if(this_room->whose_turn==p1->room_member_NO)//如果当前是自己回合
		{
			if(this_room->current_running_effect==UNO_add_card)//如果当前有加罚效果
				UNO_tips(p1,UNO_got_punish);
			else
				UNO_tips(p1,UNO_cannot_play_this_card);//进行提示，无法打出这张牌
		}
		//cout<<"最后一张牌是";cout_this_card(this_room->last_color,this_room->last_effect);
		//cout<<"可出的颜色和效果是:";cout_this_card(this_room->current_color,this_room->last_effect);
		//cout<<"你想出的是:";cout_this_card(p1->card_color[choice],p1->card_effect[choice]);
		//cout<<"出牌失败\n";
		return ;//无法使用
	}
	//执行到这里必然能打出这张卡，但要判断是否是自己回合，是的则可以，不是则不可以

	if(p1->card_color[choice]==this_room->last_color&&p1->card_effect[choice]==this_room->last_effect)//抢，无视当前是否是我的回合，若想出的牌与当前牌一致，直接出
	{
		UNO_seize_turn(this_room,p1);//强制变为我的回合,并打出那张卡
	}
	else
		if(this_room->whose_turn!=p1->room_member_NO)//若当前不是我的回合
		{
			UNO_update_all_info_to_himself(this_room,p1);//更新所有信息
			//cout<<"当前不是我的回合，无视请求,无法出牌\n";
			return;
		}

	//执行到这里必然能打出那张牌
	//封锁其他用户线程，包括机器人的，来处理打出这张牌的事件
	UNO_lock_player(this_room,p1);

	//更新当前谁打出来一张牌
	UNO_update_current_card(this_room,p1,p1->card_color[choice],p1->card_effect[choice]);

	int color=p1->card_color[choice];
	int effect=p1->card_effect[choice];

	//如果该卡有效果，发动效果，效果在发动完后需将current_running_effect重新设置为UNO_none
	switch(effect)
	{
		case UNO_ADD_2://加罚2
			this_room->current_running_effect=UNO_add_card;//当前效果设置为加罚卡片
			this_room->punish_card_number+=2;//设置加罚数量
			break;
		case UNO_ADD_4://加罚4
			this_room->current_running_effect=UNO_add_card;//当前效果设置为加罚卡片
			this_room->punish_card_number+=4;//设置加罚数量
			break;
		case UNO_reverse://逆转
			UNO_update_current_direct(this_room);
			break;
		case UNO_forbid:
			this_room->current_running_effect=UNO_forbid;//用于切换回合时检查，若有该标志符则跳过下个人的回合
			break;
	}

	//该牌送入墓地
	UNO_add_card_to_tomb(this_room,color,effect);

	//清空该卡槽
	p1->card_color[choice]=UNO_none;
	p1->card_effect[choice]=UNO_none;

	UNO_refresh_player_card_used(p1);//因为打出了一张卡，重新排列手卡

	if(color==UNO_black)//如果说是黑卡，玩家选择颜色
		UNO_player_choose_color(this_room,p1);//this_room->current_color=choice;

	

	//如果是机器人，手中只剩一张牌，打出后自动喊uno
	if(p1->card_color[1]==UNO_none&&p1->state!=UNO_human)
		UNO_declare_UNO(this_room,p1);

	//如果赢了，通知其他人游戏结束
	if(p1->card_color[0]==UNO_none)
		UNO_game_over_process(this_room,p1,true);//cout<<"有人手卡用完了!\n";
	else
		UNO_sys_change_turn(this_room);//打出一张牌，必然切换角色回合

	//处理事件完成，解放其他用户线程监听
	UNO_release_player(this_room,p1);
	return ;

}

void UNO_get_card(UNO_room* this_room,UNO_player* p1,int number,bool whether_refresh_data)//获得手卡，自主抽卡或加罚
{
	if(number<=0){cout<<"抽卡数量为0？\n";return;}

	p1->whether_get_card=true;//确认已经摸过牌

	int deck_card_number=check_how_many_card_in_deck(this_room);//确认卡组里有多少卡
	if(deck_card_number==0)//如果卡组没卡,确认墓地里有没有卡，如果没有，返回不用抽卡
	{
		int tomb_card_number=check_how_many_card_in_tomb(this_room);
		if(tomb_card_number==0)
			return;//说明墓地里也没有卡，返回不用抽卡
		else
		{
			UNO_shuffle(this_room);//说明墓地里有卡，将墓地中卡都洗到卡组
			deck_card_number=check_how_many_card_in_deck(this_room);//再次确认卡组有多少卡
		}
	}

	//到此处，卡组必然有卡，必然能抽卡
	p1->whether_declare_UNO=false;//无论之前有没有喊过UNO，失效
	deck_card_number--;//deck_card_number指向最后一个非空的卡槽

	//确认玩家有多少手卡	
	int player_card_number=check_how_many_card_in_player(p1);
	//player_card_number此时最后指向空位

	while(number>0)//当还要抽卡时
	{
		p1->card_color[player_card_number]=this_room->deck_card_color[deck_card_number];
		p1->card_effect[player_card_number]=this_room->deck_card_effect[deck_card_number];
		this_room->deck_card_color[deck_card_number]=UNO_none;//卡组内原卡槽清空
		this_room->deck_card_effect[deck_card_number]=UNO_none;//卡组内原卡槽清空
		player_card_number++;//玩家拥有卡数量+1，指向下一个空槽进行填充
		number--;//还要抽卡的数量-1
		deck_card_number--;//卡组指向最后一个非空卡槽
		if(deck_card_number==-1)//如果卡组没卡了
		{
			int tomb_card_number=check_how_many_card_in_tomb(this_room);//确认墓地里有多少卡
			if(tomb_card_number==0)break;//如果墓地也没卡了，不继续抽了
			UNO_shuffle(this_room);//确认墓地还有卡，洗牌，将墓地中卡加入卡组
		}
	}

	//如果说是加罚，不用更新数据，加罚后自动在回合切换里执行更新数据
	//如果说是首次所有人抽卡，则视为加罚，之后在翻牌时更新所有人数据
	//如果是自己选择摸牌，则更新数据
	UNO_refresh_player_card_add(p1,whether_refresh_data);//因为添加了手卡而更新他手卡，重新排序，同时通知他进行更新
}

void UNO_user_accept_punish_card(UNO_room* this_room,UNO_player* p1,int number)//角色承受加罚
{
	if(number<=0){cout<<"加罚0张？\n";return;}

	char log[100]={0};addtext(log,"承受加罚: ");addtext(log,number);addtext(log,"张!");
	UNO_game_add_log(this_room,p1,log);
	UNO_get_card(this_room,p1,number,false);//抽卡

	this_room->punish_card_number=0;//加罚数量变为0
	this_room->current_running_effect=UNO_none;//加罚效果清空
}


void UNO_user_apply_get_card(UNO_room* this_room,UNO_player* p1,int number)//角色申请获得手卡
{
	if(this_room->whose_turn!=p1->room_member_NO)//如果当前不是我的回合
	{
		return;//cout<<"不是我的回合，直接无视请求\n";
	}

	if(this_room->current_running_effect==UNO_add_card)//如果当前有加罚效果，没牌可出，不用抽卡，直接选“过”，提醒他不用抽卡，
	{
		UNO_tips(p1,UNO_got_punish_choose_pass);//cout<<"当前有加罚效果，不用抽卡，请直接选择“过”\n";
		return;
	}
	if(this_room->player[p1->room_member_NO].whether_get_card)//如果自己摸过牌
	{
		UNO_tips(p1,UNO_got_card_please_choose_pass);//cout<<"已经摸过牌，请选择"过"\n";
		return;;
	}
	//执行到这里必然能摸牌
	UNO_game_add_log(this_room,p1,"选择摸牌");	
	UNO_lock_player(this_room,p1);//事件发动，锁死其他人线程
	UNO_get_card(this_room,p1,number,true);
	UNO_release_player(this_room,p1);//事件结束，解锁其他人线程
}
void UNO_user_apply_change_turn(UNO_room* this_room,UNO_player* p1)//用户申请切换回合，如果有加罚效果，加罚，否则切换回合
{
	if(this_room->whose_turn!=p1->room_member_NO)
	{
		//cout<<"不是我的回合，直接无视请求\n";
		return;
	}
	if(p1->state==UNO_human&&p1->c1->F_offline)//如果说是活人主回合掉线
		;
	else
		if(this_room->current_running_effect!=UNO_add_card)//如果有加罚效果，不用摸牌判定
			if(!this_room->player[p1->room_member_NO].whether_get_card)//如果我没摸过牌，提醒摸牌
			{
				UNO_tips(p1,UNO_please_get_card);//cout<<p1->c1->client_name<<"没摸牌呢！\n";
				return;
			}

	//执行到这里必然能选择"过"
	UNO_lock_player(this_room,p1);
	UNO_game_add_log(this_room,p1,"选择\"过\"");
	if(p1->state==UNO_human&&p1->c1->F_offline)//如果说是活人主回合掉线
		;
	else//如果活人掉线有加罚效果，罚给下一个人，否则罚给这个人
		if(this_room->current_running_effect==UNO_add_card)//如果有加罚效果
		{
			UNO_user_accept_punish_card(this_room,p1,this_room->punish_card_number);//加罚函数
		}
	UNO_sys_change_turn(this_room);//切换回合
	UNO_release_player(this_room,p1);
}







#endif