#ifndef INCLUDE_UNO_room_process_H
#define INCLUDE_UNO_room_process_H

#include"server_general_function.h"

void UNO_clean_player_card(UNO_player* p1)//这个人手卡清空
{
	
	for(int i=0;i<UNO_deck_card_limit;i++)
	{
		p1->card_color[i]=UNO_none;
		p1->card_effect[i]=UNO_none;
	}
	
}
void UNO_ini_room()//所有房间初始化
{
	UNO_empty_room_pointer=0;//指向第一个
	int i,j;
	for(i=0;i<UNO_room_limit;i++)
	{
		uno_room[i].room_member=0;//房间内人数为0
		uno_room[i].who_is_master=-1;//房间为空
		for(j=0;j<UNO_member_limit;j++)
		{
			uno_room[i].player[j].room_member_NO=j;//自己是房间内第（从0开始）个用户
			uno_room[i].player[j].state=UNO_empty;//位子状态设为空
			uno_room[i].player[j].room_No=i;//房间号对应
			uno_room[i].player[j].whether_get_card=false;//全都变为没抽过卡状态
			uno_room[i].player[j].think_time=0;
			uno_room[i].player[j].not_declare_UNO_time=0;
			uno_room[i].player[j].whether_declare_UNO=false;
			uno_room[i].player[j].whether_choosing_color=false;
			UNO_clean_player_card(&uno_room[i].player[j]);

			uno_room[i].AI_thread[i]=NULL;//AI线程清空
			uno_room[i].game_start=false;//游戏没准备开始
			uno_room[i].game_over=true;//游戏是否结束，false为还没结束，在游戏开始后做准备工作时game_over也为true，表示游戏没结束
			uno_room[i].whether_lock=false;//没有锁死其他人
			

			//AI分析方面
			uno_room[i].player[j].only_color=UNO_none;
			uno_room[i].player[j].not_have_color=UNO_none;
			uno_room[i].player[j].i_want_play_this_color=UNO_none;//默认为啥颜色都不想出
			
			
		}
		
	}
}

void UNO_tips(UNO_player* p1,int signal)
{
	//当前没有抽卡，选择“过”，进行提示抽卡UNO_please_get_card
	//当前有加罚，选择了抽卡，提示直接选“过”UNO_got_punish_choose_pass
	//当前是自己回合，自己出牌不符合，进行提示
	//当前已经摸过牌，请选择“过”
	if(p1->state==UNO_human)
		send_msg_signal(p1->c1,signal);
	Sleep(500);
}
void UNO_update_all_info_to_himself(UNO_room* this_room,UNO_player* p1);
char* UNO_get_client_message(UNO_player* p1,int time_length)//获取客户端发送的信息，限时操作，时长time_length,,普通打牌时和选择颜色时用到，在使用之后对选择进行超时判定强制默认选择
{
	if(p1->state!=UNO_human)return "";//不是活人，c1为空指针
	client_member* c1=p1->c1;
	memset(c1->p_message,0,sizeof(c1->p_message));
	Sleep(10);
	//cout<<"游戏服务等待用户输入\n";
	int wait_time=0;
	
	while(strlen(c1->message)==0)
	{
		if(c1->F_offline)return "";
		Sleep(100);

		//如果玩家思考完作出选择后当前不是自己回合了，说明回合被抢了。说明别人打出了相同的黑色牌,则此次自己选择颜色不作数了。
		//但这种情况应该不会发生，因为打出这张牌后所有人线程锁死，可以让第一个打出黑牌的人专心选颜色，只可能超时

		if(uno_room[p1->room_No].game_over) return "";//如果游戏结束，退出监听
		
		if(p1->card_color[1]==UNO_none&&p1->whether_declare_UNO==false)//如果没在限定时间内没喊uno，加罚2张卡
		{
			p1->not_declare_UNO_time+=100;
			if(p1->not_declare_UNO_time>=UNO_declare_time_limit)
			{
				//cout<<"没喊uno，加罚2张!\n";
				return "";
			}
			
		}
		else
			p1->not_declare_UNO_time=0;

		if(uno_room[p1->room_No].whose_turn==p1->room_member_NO)//如果当前是自己回合,限时等待
		{
			p1->think_time+=100;
			if(p1->think_time==2000||p1->think_time%10000==0)//反复每隔一段时间刷新，防止客户端没收到消息
				UNO_update_all_info_to_himself(&uno_room[p1->c1->room_No],p1);
			if(p1->think_time>=time_length)//如果等待超时
			{
				//cout<<"用户输入超时!\n";
				return "";
			}
		}
		else
			p1->think_time=0;
	}//进行等待

	//执行到这里必然收到数据
	addtext(c1->p_message,c1->message);
	c1->not_be_read=false;
	Sleep(5);
	//cout<<"get msg:"<<c1->p_message<<endl;
	return c1->p_message;
}


void UNO_add_room_message(UNO_room* this_room,JSON_package* json_msg)//添加房内成员信息到json
{
	char StrBuf[MAX_PATH] = { 0 };
	json_msg->add_item("master_number",this_room->who_is_master);//谁是房主
	for(int count=0;count<UNO_member_limit;count++)//统计房间内人员数据
	{
		switch(this_room->player[count].state)
		{
			case UNO_empty://addtext(Str_member,"【空】 ");
				json_msg->add_item_array("room_member","name","【空】");
				break;
			case UNO_human:
				memset(StrBuf,0,sizeof(StrBuf));
				if(this_room->who_is_master==count)addtext(StrBuf,"【房主】");
				addtext(StrBuf,this_room->player[count].c1->client_name);
				json_msg->add_item_array("room_member","name",StrBuf);
				break;
			case UNO_simple_AI:
				//addtext(Str_member,"【简单机器人】 ");
				json_msg->add_item_array("room_member","name","简单机器人");
				
				break;
			case UNO_hard_AI:
				memset(StrBuf,0,sizeof(StrBuf));addtext(StrBuf,"一般机器人");addtext(StrBuf,this_room->player[count].room_member_NO+1);addtext(StrBuf,"号");
				json_msg->add_item_array("room_member","name",StrBuf);
				
				break;
			default:
				json_msg->add_item_array("room_member","name","奇怪东西");
				break;
		}
	}
}

void UNO_refresh_room_member_to_all(int signal,UNO_room* this_room,client_member* c1)//更新房间内人员数据，发起事件者,发起事件
{
	//UNO_add_full_robot_success
	//UNO_create_room_success
	//UNO_enter_room_success
	//UNO_someone_leave_room

	JSON_package json_msg,json_msg2;
	json_msg.add_item("event_name",c1->client_name);//发动事件者姓名
	json_msg2.add_item("event_name",c1->client_name);//发动事件者姓名

	json_msg.add_item("signal",signal);
	switch(signal)
	{
		case UNO_already_in_room:
			json_msg2.add_item("signal",UNO_refresh_room_member);
			break;
		case UNO_add_full_robot_success:
			json_msg2.add_item("signal",UNO_add_full_robot_success);
			break;
		case UNO_create_room_success:
			break;
		case UNO_enter_room_success:
			json_msg2.add_item("signal",UNO_someone_get_in);
			break;
		case UNO_leave_room_success:
			json_msg2.add_item("signal",UNO_someone_leave_room);
			break;
	}
	
	
	UNO_add_room_message(this_room,&json_msg);//添加房间所有成员信息
	UNO_add_room_message(this_room,&json_msg2);

	client_member* c2;
	for(int i=0;i<UNO_member_limit;i++)
		if(this_room->player[i].state==UNO_human)//如果是活人，发送消息给他
		{
			c2=this_room->player[i].c1;
			if(c2->member_No==c1->member_No)
			{
				json_msg.add_item("your_number",i);
				send_msg(c2,json_msg.to_StrBuf());//告诉他所有成员信息
			}
			else
			{
				json_msg2.add_item("your_number",i);
				send_msg(c2,json_msg2.to_StrBuf());//告诉他所有成员信息
			}

			
		}
}

void UNO_add_FULL_robot_to_room(client_member* c1)//如果是房主，进行添加机器人
{

	int who_is_master=uno_room[c1->room_No].who_is_master;//获取房主序号
	client_member* master;
	master=uno_room[c1->room_No].player[who_is_master].c1;//确认房主角色
	if(who_is_master==-1){cout<<"房主没有？\n";return;}
	if(master!=NULL)//
	{
		if(uno_room[c1->room_No].player[who_is_master].c1->member_No!=c1->member_No)//如果不是房间主人，无权限操作并返回
		{
			send_msg_signal(c1,UNO_not_got_room_operate_right);
			return;
		}
	}
	else
	{cout<<"房主为空？"<<who_is_master<<endl;return;}
	
	uno_room[c1->room_No].room_member=UNO_member_limit;//直接设置为满员
	for(int i=0;i<UNO_member_limit;i++)
		if(uno_room[c1->room_No].player[i].state==UNO_empty)
			uno_room[c1->room_No].player[i].state=UNO_hard_AI;//角色变更为一般机器人
	UNO_refresh_room_member_to_all(UNO_add_full_robot_success,&uno_room[c1->room_No],c1);
	

}

void UNO_create_room(client_member* c1)//用户创建房间
{
	if(UNO_room_in_use==UNO_room_limit)//如果房间全满
	{send_msg_signal(c1,UNO_room_full);return;}

	int this_room_number=UNO_empty_room_pointer;
	
	while(uno_room[this_room_number].who_is_master!=-1)
	{
		cout<<"UNO 错误！！！\n";
		Sleep(100);
		this_room_number=UNO_empty_room_pointer;
		if(UNO_room_in_use==UNO_room_limit)//创建失败!
		{send_msg_signal(c1,UNO_room_full);return;}
	}

	//到这一步必然能创建房间
	uno_room[this_room_number].who_is_master=0;//当前房间房主设置为位置0者
	UNO_room_in_use++;

	int temp=UNO_empty_room_pointer;
	if(UNO_room_in_use!=UNO_room_limit)//如果房间没满
		while(true)//搜寻下一个空房间，赋予UNO_empty_room_pointer
		{
			temp++;//看下一个房间
			if(temp==UNO_room_limit)temp=0;//若到达最后一个房间，从头开始找

			if(uno_room[temp].who_is_master==-1)//如果该房间为空
			{
				UNO_empty_room_pointer=temp;//确认房间号，赋予UNO_empty_room_pointer
				break;//跳出循环
			}
		
		}

	uno_room[this_room_number].player[0].c1=c1;
	uno_room[this_room_number].player[0].state=UNO_human;//该位子状态设置为人类玩家

	c1->room_No=this_room_number;//确认玩家所在房间号
	c1->guest=false;
	UNO_refresh_room_member_to_all(UNO_create_room_success,&uno_room[this_room_number],c1);
	c1->state=UNO_in_room;//更新玩家状态为“在uno房间内”
	uno_room[this_room_number].room_member++;//房间内人数为1
	
}


void UNO_show_room_in_use(client_member* c1)//向他展示所有能加入的房间
{
	if(UNO_room_in_use==0)//如果没有使用着的房间
	{send_msg_signal(c1,UNO_no_room_in_use);return;}

	JSON_package json_msg;
	json_msg.add_item("signal",UNO_show_roomlist);
	JSON_package room_msg;

	for(int count=0;count<UNO_room_limit;count++)
		if(uno_room[count].who_is_master!=-1)
		{
			room_msg.add_item("room_No",count+1);//房间号
			room_msg.add_item("master",uno_room[count].player[uno_room[count].who_is_master].c1->client_name);//房主姓名+人数
			room_msg.add_item("room_member",uno_room[count].room_member);//房间人数

			json_msg.add_struct_array("room_msg",room_msg.get_json());
		}
	send_msg(c1,json_msg.to_StrBuf());//发送
}

void UNO_enter_room(client_member* c1,JSON_package json_msg)//用户进入别人的房间
{
	if(UNO_room_in_use==0)//如果没有使用着的房间
	{send_msg_signal(c1,UNO_no_room_in_use);return;}
	

	//执行到这里必然能选择别人的房间并加入

	int choice=atoi(json_msg.get_value("which_room"));//玩家作出的选择转换数据为数字 
						
		if(choice==0)//confirm_send_success(c1,"输入了无效值!\n");
		{return;}
		if(choice>UNO_room_limit||choice<0)//confirm_send_success(c1,"输入超出房间数量上限!\n");
		{return;}

		choice--;
		
		if(uno_room[choice].who_is_master==-1)//confirm_send_success(c1,"该房间内没人!\n");
		{send_msg_signal(c1,UNO_this_room_is_empty);return;}
		if(uno_room[choice].room_member==UNO_member_limit)//confirm_send_success(c1,"该房间内人满!\n");
		{send_msg_signal(c1,UNO_this_room_is_full);return;}
		if(uno_room[choice].game_start==false&&uno_room[choice].game_over==false)//里面正在玩游戏，还没结束
		{send_msg_signal(c1,UNO_room_is_in_game);return;}
		
		//执行到这一步必然加入成功
		uno_room[choice].room_member++;//房间内人数+1
		c1->guest=true;//加入房间的人是房间客人
		c1->room_No=choice;
		c1->state=UNO_in_room;//更新玩家状态为“在uno房间内”

		for(int i=0;i<UNO_member_limit;i++)
			if(uno_room[choice].player[i].state==UNO_empty)//如果当前位子为空
			{
				uno_room[choice].player[i].state=UNO_human;//该位子状态设置为人类玩家
				
				uno_room[choice].player[i].c1=c1;//标识
				GetLocalTime(&uno_room[choice].player[i].enter_time);//截取当前时间，用于房主变动
				break;
			}

		c1->room_No=choice;//保存房间号信息
		UNO_refresh_room_member_to_all(UNO_enter_room_success,&uno_room[choice],c1);//刷新成员
		c1->state=UNO_in_room;

}

bool compare_enter_time(SYSTEMTIME challenger,SYSTEMTIME former_master)//返回true表示前者比后者大
{
	bool result=false;//不比房主候补早
	if(challenger.wYear<former_master.wYear)
		result=true;
	else
	{
		if(challenger.wMonth<former_master.wMonth)
			result=true;
		else
		{
			if(challenger.wDay<former_master.wDay)
				result=true;
			else
			{
				if(challenger.wHour<former_master.wHour)
					result=true;
				else
				{
					if(challenger.wMinute<former_master.wMinute)
						result=true;
					else
						if(challenger.wMilliseconds<former_master.wMilliseconds)
							result=true;
				}
			}
		}
	}
	return result;
}





void UNO_exit_room(client_member* c1)//用户退出房间
{
	
	char StrBuf[MAX_PATH] = { 0 };

	int who_will_be_the_master=-1;//如果房主变更，这个会触发


	UNO_room* this_room=&uno_room[c1->room_No];
	
	for(int i=0;i<UNO_member_limit;i++)
		if(this_room->player[i].c1!=NULL&&this_room->player[i].c1->member_No==c1->member_No)//找到要退出者
		{
			
			if(this_room->who_is_master==i)//如果退出者为房间主人,更换房主措施
			{
				for(int j=UNO_member_limit-1;j>=0;j--)
					if(i!=j&&this_room->player[j].state==UNO_human)//对所有位子检查，若当前位置上是活人
					{
						if(who_will_be_the_master==-1)//如果当前还没确认过房主候补，将当前位置上的人作为房主候补
							who_will_be_the_master=j;
						else//如果有房主候补了，比较两者的进入房间时间
						{
							if(compare_enter_time(this_room->player[j].enter_time,this_room->player[who_will_be_the_master].enter_time))
								who_will_be_the_master=j;
						}

					}
				this_room->who_is_master=who_will_be_the_master;//确认新房主
				if(who_will_be_the_master!=-1)//若当前房间不为空
					this_room->player[who_will_be_the_master].c1->guest=false;
				else//没人了,该房间没人使用
				{
					if(this_room->game_start)//如果说游戏正在进行中
					{
						this_room->game_over=true;
						Sleep(600);//等待AI线程结束
						this_room->game_start=false;
					}
					for(int k=0;k<UNO_member_limit;k++)
					{
						this_room->player[k].state=UNO_empty;//位子状态设为空,机器人清空
					}
					this_room->room_member=0;//直接清0，因为机器人也算人数
					UNO_room_in_use--;//总房间使用数量-1
				}
			}//房主变更手续结束
			
			this_room->player[i].c1=NULL;//两者取消关联
			
			if(this_room->who_is_master!=-1)//如果房间内还有人
				this_room->room_member--;//房间内人数-1
			else//房间内没人了
			{
				//一般没人了说明是房主离开房间，房间内没人结算在房主变更手续中执行
			}
			c1->guest=false;
			this_room->player[i].state=UNO_empty;//当前位子状态设置为空

			send_msg_signal(c1,UNO_leave_room_success);//离开成功!
			c1->state=in_online;//更改用户状态为“在线”

			//该成员离房手续办理结束，通知房间中其他人该角色离去
			if(uno_room[c1->room_No].room_member!=0)//如果房间中还有人
			{
				UNO_refresh_room_member_to_all(UNO_leave_room_success,this_room,c1);//让他更新房间内人员数据
			}
			
			
			break;
		}


}




#endif