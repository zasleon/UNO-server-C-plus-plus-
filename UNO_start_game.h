#ifndef INCLUDE_UNO_start_game_H
#define INCLUDE_UNO_start_game_H

#include "UNO_AI.h"

void UNO_game_ini(UNO_room* this_room)//游戏开始的各种初始化工作
{
	UNO_ini_deck(this_room);//初始化卡组
	this_room->whose_turn=-1;//当前还没确认是谁的回合
	this_room->clock_direct=true;//初始默认为顺时针发牌
	this_room->current_running_effect=UNO_none;
	this_room->punish_card_number=0;
	for(int i=0;i<UNO_member_limit;i++)
	{
		if(this_room->player[i].state!=UNO_empty)//座位上不为空的给他发牌
		{
			if(this_room->player[i].state==UNO_human)//如果座位上是活人
			{
				this_room->player[i].c1->state=UNO_in_game;//状态修改为“在UNO游戏中”
			}
			else//如果是机器人，开启机器人思考线程
			{
				cout<<"开启机器人\n";
				if(this_room->AI_thread[i]!=NULL)
					CloseHandle(this_room->AI_thread[i]);//防止句柄被用完
				this_room->AI_thread[i]=CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)UNO_AI_think,&this_room->player[i], NULL, NULL);//开启AI思考线程
			}

			this_room->player[i].whether_get_card=false;//全都变为没抽过卡状态
			this_room->player[i].think_time=0;
			this_room->player[i].not_declare_UNO_time=0;
			this_room->player[i].whether_declare_UNO=false;
			this_room->player[i].whether_choosing_color=false;
			UNO_clean_player_card(&this_room->player[i]);//将该座位上角色手卡清空

		}//对该座位上的角色处理结束
	}//对所有角色处理结束
}




int UNO_calculate_card_score(UNO_player* p1)//计算手牌分值
{
	int score=0;
	for(int i=0;i<UNO_deck_card_limit;i++)
	{
		if(p1->card_color[i]==UNO_none)//没卡了，破除循环返回数值
			break;
		if(p1->card_color[i]==UNO_black)//黑色万能牌价值50
		{
			score+=50;
			continue;
		}
		switch(p1->card_effect[i])//功能牌+20，数值牌+数值
		{
			case UNO_universal:cout<<"怎么会有万能牌???\n";break;
			case UNO_ADD_2:
			case UNO_ADD_4:
			case UNO_reverse:
			case UNO_forbid:
				score+=20;
			default:
				score+=p1->card_effect[i];
		}
	}
	return score;
}

void UNO_decide_first_whose_turn(UNO_room* this_room)//决定从谁先开始
{
		int score[UNO_member_limit];//用于计算每个人手牌分值，最低者开牌
		for(int i=0;i<UNO_member_limit;i++)
		{
			score[i]=-1;
		}
		
		int now_is_whose_turn=0;//还不确定当前是谁的回合，默认为0号（第一个）位上的角色
		int lowest_value=9999;//当前最低值

		for(int i=0;i<UNO_member_limit;i++)
		{
			if(this_room->player[i].state!=UNO_empty)//座位上不为空的给他发牌
			{
				UNO_get_card(this_room,&this_room->player[i],7,false);//每个人发7张牌,不用更新数据
				score[i]=UNO_calculate_card_score(&this_room->player[i]);//计算该玩家手牌分值

				if(score[i]<lowest_value&&score[i]!=-1)//如果该位子上的玩家不为空且手牌分值比之前几个玩家的分值少
				{
					lowest_value=score[i];//更新最低值
					now_is_whose_turn=i;//拟定该玩家为先手出牌者
				}
			}//对该座位上的角色处理结束
		}//对所有角色处理结束


		this_room->whose_turn=now_is_whose_turn;//确认完了是谁的回合
}




void UNO_punish_for_not_declare_UNO(UNO_room* this_room,UNO_player* p1)
{
	UNO_game_add_log(this_room,p1,"没喊UNO!加罚2张!");
	UNO_get_card(this_room,p1,2,true);//抽卡,需要更新数据
}


void UNO_player_choose_color(UNO_room* this_room,UNO_player* p1)//因为出了黑色牌，玩家选择之后玩家要出的颜色
{
	p1->whether_choosing_color=true;

	if(p1->state!=UNO_human)//判断为AI
	{
		UNO_AI_which_color_i_wanna_play(this_room,p1);
		if(this_room->current_color==UNO_none){cout<<"机器选择了奇怪颜色???????\n";}
		UNO_update_current_color(this_room,p1,p1->i_want_play_this_card);//广播当前可以出的颜色

		if(p1->card_color[1]==UNO_none)
			p1->only_color=this_room->current_color;
		return;
	}
	
	char StrBuf[MAX_PATH] = { 0 };
	while(true)//选择颜色死循环
	{
		send_msg_signal(p1->c1,UNO_please_choose_color);
		memset(StrBuf,0,sizeof(StrBuf));
		addtext(StrBuf,UNO_get_client_message(p1,UNO_user_choose_time_limit));
		if(p1->c1->F_offline||p1->think_time>=UNO_user_choose_time_limit)//如果用户在有限思考时间内没做出选择，强制选择绿色
		{
			UNO_game_add_log(this_room,p1,"选择颜色超时!强制选择默认颜色!");
			UNO_update_current_color(this_room,p1,UNO_green);//广播当前可以出的颜色
			break;//破除选择颜色死循环
		}
		if(p1->not_declare_UNO_time>=UNO_declare_time_limit)//玩家要先选完颜色后才能说UNO
		{
			UNO_punish_for_not_declare_UNO(this_room,p1);//加罚函数
			continue;
		}

		//如果玩家思考完作出选择后当前不是自己回合了，说明回合被抢了。说明别人打出了相同的黑色牌,则此次自己选择颜色不作数了。
		//但这种情况应该不会发生，因为打出这张牌后所有人线程锁死，可以让第一个打出黑牌的人专心选颜色，只可能超时

		switch(atoi(StrBuf))
		{
			case UNO_green:
			case UNO_red:
			case UNO_blue:
			case UNO_yellow:
				UNO_update_current_color(this_room,p1,atoi(StrBuf));//广播当前可以出的颜色
				break;
			default://如果出现错误情况，重新让他选择颜色
				cout<<"选择了错误的颜色:"<<StrBuf<<endl;
				continue;
		}

		break;//破除选择颜色死循环
	}
	
	p1->whether_choosing_color=false;
	if(p1->card_color[1]==UNO_none)
		p1->only_color=this_room->current_color;
	

}



void UNO_flop(UNO_room* this_room)//翻开卡组第一张牌
{
	//确认卡组最上方那张牌
	int pointer=UNO_deck_card_limit-1;
	for(;pointer>=0;pointer--)
		if(this_room->deck_card_color[pointer]!=UNO_none)
			break;
	//指向最后一个有卡的卡槽

	
	//UNO_update_current_card(this_room,NULL,UNO_black,UNO_ADD_4);//首个是黑+4，测试时使用
	//UNO_update_current_card(this_room,NULL,UNO_black,UNO_universal);//首个是黑万能，测试时使用
	//this_room->player[0].card_color[0]=UNO_black;this_room->player[0].card_effect[0]=UNO_universal;//自己第一张变为万能牌

	//将当前牌作为翻牌后的牌
	UNO_update_current_card(this_room,NULL,this_room->deck_card_color[pointer],this_room->deck_card_effect[pointer]);
	char log_content[300]={0};
	addtext(log_content,"首次翻牌为:\n【");
	addtext(log_content,UNO_get_this_card_color(this_room->last_color));
	addtext(log_content,UNO_get_this_card_effect(this_room->last_effect));addtext(log_content,"】!");
	UNO_game_add_log(this_room,&this_room->player[this_room->whose_turn],log_content);//记录翻拍日志
	
	//该牌送入墓地
	this_room->deck_card_color[pointer]=UNO_none;
	this_room->deck_card_effect[pointer]=UNO_none;
	UNO_add_card_to_tomb(this_room,this_room->last_color,this_room->last_effect);

	

	//若是功能牌，发动效果
	switch(this_room->last_color)
	{
		case UNO_black://由玩家选择当前颜色变化
			UNO_player_choose_color(this_room,&this_room->player[this_room->whose_turn]);
			break;
		default:
			break;
	}
	switch(this_room->last_effect)
	{
		case UNO_ADD_2:
			this_room->current_running_effect=UNO_add_card;
			this_room->punish_card_number=2;
			break;
		case UNO_ADD_4:
			this_room->current_running_effect=UNO_add_card;
			this_room->punish_card_number=4;
			break;
		case UNO_reverse:
			UNO_update_current_direct(this_room);
			break;
		case UNO_forbid:
			UNO_game_add_log(this_room,&this_room->player[this_room->whose_turn],"的回合被跳过了!");
			UNO_sys_change_turn(this_room);
			break;
		default:
			break;
	}
	
	
	
}



void UNO_send_game_start_signal(UNO_room* this_room)
{
	char StrBuf[MAX_PATH] = { 0 };
	JSON_package json_msg;
	json_msg.add_item("signal",UNO_game_start);

	UNO_add_room_message(this_room,&json_msg);//添加房间成员信息

	for(int i=0;i<UNO_member_limit;i++)
		if(this_room->player[i].state==UNO_human)
		{
			json_msg.add_item("your_number",i);
			send_msg(this_room->player[i].c1,json_msg.to_StrBuf());//告诉他所有成员信息、开始游戏了
		}
}




void UNO_start_one_game(client_member* c1,bool whether_positive)
{
	
	if(whether_positive)//如果是主动申请开始游戏，判断可不可以开始，如果是被动的则直接开始游戏
	{
		if(c1->guest)//如果c1不是房主，无权开启游戏
			return;
		if(uno_room[c1->room_No].room_member<=1)//如果房间内只有一个人，告诉他人数不足
		{
			send_msg_signal(c1,UNO_lack_of_players);
			return;
		}
	}//判断结束，之后步骤确认游戏开始

	UNO_room* this_room=&uno_room[c1->room_No];
	UNO_player* p1=NULL;//获取c1用户的角色p1
	int my_place=-1;
	for(int i=0;i<UNO_member_limit;i++)
		if(uno_room[c1->room_No].player[i].c1->member_No==c1->member_No)
		{
			p1=&uno_room[c1->room_No].player[i];
			my_place=i;
			break;
		}
	
	char StrBuf[MAX_PATH] = { 0 };

	if(whether_positive)
	{
		this_room->game_over=false;//游戏并没有结束
		UNO_send_game_start_signal(this_room);//告诉玩家游戏开始了
		UNO_game_ini(this_room);
		UNO_decide_first_whose_turn(this_room);//确定最先是谁的回合
		UNO_flop(this_room);//翻牌，将效果施加给先手者，将翻拍后的卡面消息广播

		//准备完了，通知其他人
		this_room->game_start=true;//游戏开始了
		Sleep(200);//保证所有机器人都不再等待游戏标识符，确保他们能进入游戏

	}
	else//如果不是启动游戏的活人，等待游戏开始，AI在其线程内也等待游戏开始
		while(!this_room->game_start)
			Sleep(200);
	

	while(true)
	{
		if(this_room->game_over)return;//如果游戏结束了，回到原来界面
		
		memset(StrBuf,0,sizeof(StrBuf));
		addtext(StrBuf,UNO_get_client_message(p1,UNO_user_choose_time_limit));
		if(this_room->game_over){return;}//如果游戏结束了，回到原来界面

		if(p1->card_color[1]==UNO_none&&p1->not_declare_UNO_time>=UNO_declare_time_limit)//没在限定时间内喊UNO
		{
			UNO_lock_player(this_room,p1);//事件发动，锁死其他人线程
			UNO_punish_for_not_declare_UNO(this_room,p1);//加罚函数
			UNO_release_player(this_room,p1);//事件结束，解锁其他人线程
			continue;
		}
		
		if(p1->think_time>=UNO_user_choose_time_limit)//如果用户在有限思考时间内没做出选择，强制抽1卡过
		{
			UNO_game_add_log(this_room,p1,"思考超时!强制摸牌并结束回合!");
			UNO_user_apply_get_card(this_room,p1,1);//里面会判定，如果有加罚就不用抽卡，直接请用户选择“过”，“过”的函数里面会自动执行加罚
			UNO_user_apply_change_turn(this_room,p1);//加罚直接在这里面执行
			continue;
		}

		if(c1->F_offline)//接收客户端发来消息，如果此时断开通讯结束沟通
		{
			UNO_lock_player(this_room,p1);//离线事件发生，锁住大家行动

			UNO_game_add_log(this_room,p1,"退出了游戏!");
			int human_number=0;//统计房间活人人数
				for(int i=0;i<UNO_member_limit;i++)
				{
					if(this_room->player[i].state==UNO_human)
						human_number++;
				}

			if(human_number==1)//若房间里只有我一个活人，而我又掉线了，则房间里只剩机器人了，直接退出游戏函数，之后将在其他函数与主界面函数执行退房离线等操作
			{
				//结束游戏
			}
			else//如果说房间里还有别的活人
			{
				int player_number=0;//统计房间游戏人数（包括机器人）
				for(int i=0;i<UNO_member_limit;i++)
				{
					if(this_room->player[i].state!=UNO_empty)
						player_number++;
				}
				
				if(player_number<=2)//如果房间里只有另一个活人了，没机器人，则结束游戏，如果有机器人则继续游戏，自己直接return不妨碍任何游戏进程
					UNO_game_over_process(this_room,p1,false);
				else//有许多人，则只切换自己的回合，且丢光手卡
				{
					for(int i=0;i<UNO_deck_card_limit;i++)//把他手卡全放入墓地
						if(p1->card_color[i]!=UNO_none)
						{
							UNO_add_card_to_tomb(this_room,p1->card_color[i],p1->card_effect[i]);
							p1->card_color[i]=UNO_none;
							p1->card_effect[i]=UNO_none;
						}
						else
							break;
					if(this_room->whose_turn==p1->room_member_NO)//如果当前是我出牌的回合而我掉线了
					{
						UNO_sys_change_turn(this_room);//切换回合
					}
				}
			}
			
			UNO_release_player(this_room,p1);//离线事件结束
			
				
			return;
		}

		if(this_room->game_over)return;//如果游戏结束了，回到原来界面


		JSON_package json_msg(StrBuf);
		int choice=atoi(json_msg.get_value("signal"));
		switch(choice)
		{
			case UNO_apply_get_card://申请抽卡
				UNO_user_apply_get_card(this_room,p1,1);//里面会判定，如果有加罚就不用抽卡，直接请用户选择“过”，“过”的函数里面会自动执行加罚
				
				break;
			case UNO_i_only_got_one_card:
				if(p1->card_color[1]!=UNO_none)
				{
					UNO_update_all_info_to_himself(this_room,p1);//更新所有信息
					

					//cout<<"若有两张以上手卡，无视请求\n";
					break;
				}
				UNO_declare_UNO(this_room,p1);
				//广播uno消息
				break;
			case UNO_apply_pass://申请“过”
				UNO_user_apply_change_turn(this_room,p1);//加罚直接在这里面执行
				
				break;
			case UNO_apply_play_this_card://出牌
				//cout<<"活人选择出牌\n";
				UNO_play_this_card(this_room,p1,atoi(json_msg.get_value("which_card")));//出牌，该函数内包含判定能不能出打出这张牌，该函数里会执行回合切换
				break;
			
			default:
				cout<<"奇怪请求\n";
				break;
			
		}
			

	}

	

	

}

#endif