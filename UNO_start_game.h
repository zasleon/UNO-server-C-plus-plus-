#ifndef INCLUDE_UNO_start_game_H
#define INCLUDE_UNO_start_game_H

#include "UNO_AI.h"

void UNO_game_ini(UNO_room* this_room)//��Ϸ��ʼ�ĸ��ֳ�ʼ������
{
	UNO_ini_deck(this_room);//��ʼ������
	this_room->whose_turn=-1;//��ǰ��ûȷ����˭�Ļغ�
	this_room->clock_direct=true;//��ʼĬ��Ϊ˳ʱ�뷢��
	this_room->current_running_effect=UNO_none;
	this_room->punish_card_number=0;
	for(int i=0;i<UNO_member_limit;i++)
	{
		if(this_room->player[i].state!=UNO_empty)//��λ�ϲ�Ϊ�յĸ�������
		{
			if(this_room->player[i].state==UNO_human)//�����λ���ǻ���
			{
				this_room->player[i].c1->state=UNO_in_game;//״̬�޸�Ϊ����UNO��Ϸ�С�
			}
			else//����ǻ����ˣ�����������˼���߳�
			{
				cout<<"����������\n";
				if(this_room->AI_thread[i]!=NULL)
					CloseHandle(this_room->AI_thread[i]);//��ֹ���������
				this_room->AI_thread[i]=CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)UNO_AI_think,&this_room->player[i], NULL, NULL);//����AI˼���߳�
			}

			this_room->player[i].whether_get_card=false;//ȫ����Ϊû�����״̬
			this_room->player[i].think_time=0;
			this_room->player[i].not_declare_UNO_time=0;
			this_room->player[i].whether_declare_UNO=false;
			this_room->player[i].whether_choosing_color=false;
			UNO_clean_player_card(&this_room->player[i]);//������λ�Ͻ�ɫ�ֿ����

		}//�Ը���λ�ϵĽ�ɫ�������
	}//�����н�ɫ�������
}




int UNO_calculate_card_score(UNO_player* p1)//�������Ʒ�ֵ
{
	int score=0;
	for(int i=0;i<UNO_deck_card_limit;i++)
	{
		if(p1->card_color[i]==UNO_none)//û���ˣ��Ƴ�ѭ��������ֵ
			break;
		if(p1->card_color[i]==UNO_black)//��ɫ�����Ƽ�ֵ50
		{
			score+=50;
			continue;
		}
		switch(p1->card_effect[i])//������+20����ֵ��+��ֵ
		{
			case UNO_universal:cout<<"��ô����������???\n";break;
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

void UNO_decide_first_whose_turn(UNO_room* this_room)//������˭�ȿ�ʼ
{
		int score[UNO_member_limit];//���ڼ���ÿ�������Ʒ�ֵ������߿���
		for(int i=0;i<UNO_member_limit;i++)
		{
			score[i]=-1;
		}
		
		int now_is_whose_turn=0;//����ȷ����ǰ��˭�Ļغϣ�Ĭ��Ϊ0�ţ���һ����λ�ϵĽ�ɫ
		int lowest_value=9999;//��ǰ���ֵ

		for(int i=0;i<UNO_member_limit;i++)
		{
			if(this_room->player[i].state!=UNO_empty)//��λ�ϲ�Ϊ�յĸ�������
			{
				UNO_get_card(this_room,&this_room->player[i],7,false);//ÿ���˷�7����,���ø�������
				score[i]=UNO_calculate_card_score(&this_room->player[i]);//�����������Ʒ�ֵ

				if(score[i]<lowest_value&&score[i]!=-1)//�����λ���ϵ���Ҳ�Ϊ�������Ʒ�ֵ��֮ǰ������ҵķ�ֵ��
				{
					lowest_value=score[i];//�������ֵ
					now_is_whose_turn=i;//�ⶨ�����Ϊ���ֳ�����
				}
			}//�Ը���λ�ϵĽ�ɫ�������
		}//�����н�ɫ�������


		this_room->whose_turn=now_is_whose_turn;//ȷ��������˭�Ļغ�
}




void UNO_punish_for_not_declare_UNO(UNO_room* this_room,UNO_player* p1)
{
	UNO_game_add_log(this_room,p1,"û��UNO!�ӷ�2��!");
	UNO_get_card(this_room,p1,2,true);//�鿨,��Ҫ��������
}


void UNO_player_choose_color(UNO_room* this_room,UNO_player* p1)//��Ϊ���˺�ɫ�ƣ����ѡ��֮�����Ҫ������ɫ
{
	p1->whether_choosing_color=true;

	if(p1->state!=UNO_human)//�ж�ΪAI
	{
		UNO_AI_which_color_i_wanna_play(this_room,p1);
		if(this_room->current_color==UNO_none){cout<<"����ѡ���������ɫ???????\n";}
		UNO_update_current_color(this_room,p1,p1->i_want_play_this_card);//�㲥��ǰ���Գ�����ɫ

		if(p1->card_color[1]==UNO_none)
			p1->only_color=this_room->current_color;
		return;
	}
	
	char StrBuf[MAX_PATH] = { 0 };
	while(true)//ѡ����ɫ��ѭ��
	{
		send_msg_signal(p1->c1,UNO_please_choose_color);
		memset(StrBuf,0,sizeof(StrBuf));
		addtext(StrBuf,UNO_get_client_message(p1,UNO_user_choose_time_limit));
		if(p1->c1->F_offline||p1->think_time>=UNO_user_choose_time_limit)//����û�������˼��ʱ����û����ѡ��ǿ��ѡ����ɫ
		{
			UNO_game_add_log(this_room,p1,"ѡ����ɫ��ʱ!ǿ��ѡ��Ĭ����ɫ!");
			UNO_update_current_color(this_room,p1,UNO_green);//�㲥��ǰ���Գ�����ɫ
			break;//�Ƴ�ѡ����ɫ��ѭ��
		}
		if(p1->not_declare_UNO_time>=UNO_declare_time_limit)//���Ҫ��ѡ����ɫ�����˵UNO
		{
			UNO_punish_for_not_declare_UNO(this_room,p1);//�ӷ�����
			continue;
		}

		//������˼��������ѡ���ǰ�����Լ��غ��ˣ�˵���غϱ����ˡ�˵�����˴������ͬ�ĺ�ɫ��,��˴��Լ�ѡ����ɫ�������ˡ�
		//���������Ӧ�ò��ᷢ������Ϊ��������ƺ��������߳������������õ�һ��������Ƶ���ר��ѡ��ɫ��ֻ���ܳ�ʱ

		switch(atoi(StrBuf))
		{
			case UNO_green:
			case UNO_red:
			case UNO_blue:
			case UNO_yellow:
				UNO_update_current_color(this_room,p1,atoi(StrBuf));//�㲥��ǰ���Գ�����ɫ
				break;
			default://������ִ����������������ѡ����ɫ
				cout<<"ѡ���˴������ɫ:"<<StrBuf<<endl;
				continue;
		}

		break;//�Ƴ�ѡ����ɫ��ѭ��
	}
	
	p1->whether_choosing_color=false;
	if(p1->card_color[1]==UNO_none)
		p1->only_color=this_room->current_color;
	

}



void UNO_flop(UNO_room* this_room)//���������һ����
{
	//ȷ�Ͽ������Ϸ�������
	int pointer=UNO_deck_card_limit-1;
	for(;pointer>=0;pointer--)
		if(this_room->deck_card_color[pointer]!=UNO_none)
			break;
	//ָ�����һ���п��Ŀ���

	
	//UNO_update_current_card(this_room,NULL,UNO_black,UNO_ADD_4);//�׸��Ǻ�+4������ʱʹ��
	//UNO_update_current_card(this_room,NULL,UNO_black,UNO_universal);//�׸��Ǻ����ܣ�����ʱʹ��
	//this_room->player[0].card_color[0]=UNO_black;this_room->player[0].card_effect[0]=UNO_universal;//�Լ���һ�ű�Ϊ������

	//����ǰ����Ϊ���ƺ����
	UNO_update_current_card(this_room,NULL,this_room->deck_card_color[pointer],this_room->deck_card_effect[pointer]);
	char log_content[300]={0};
	addtext(log_content,"�״η���Ϊ:\n��");
	addtext(log_content,UNO_get_this_card_color(this_room->last_color));
	addtext(log_content,UNO_get_this_card_effect(this_room->last_effect));addtext(log_content,"��!");
	UNO_game_add_log(this_room,&this_room->player[this_room->whose_turn],log_content);//��¼������־
	
	//��������Ĺ��
	this_room->deck_card_color[pointer]=UNO_none;
	this_room->deck_card_effect[pointer]=UNO_none;
	UNO_add_card_to_tomb(this_room,this_room->last_color,this_room->last_effect);

	

	//���ǹ����ƣ�����Ч��
	switch(this_room->last_color)
	{
		case UNO_black://�����ѡ��ǰ��ɫ�仯
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
			UNO_game_add_log(this_room,&this_room->player[this_room->whose_turn],"�Ļغϱ�������!");
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

	UNO_add_room_message(this_room,&json_msg);//��ӷ����Ա��Ϣ

	for(int i=0;i<UNO_member_limit;i++)
		if(this_room->player[i].state==UNO_human)
		{
			json_msg.add_item("your_number",i);
			send_msg(this_room->player[i].c1,json_msg.to_StrBuf());//���������г�Ա��Ϣ����ʼ��Ϸ��
		}
}




void UNO_start_one_game(client_member* c1,bool whether_positive)
{
	
	if(whether_positive)//������������뿪ʼ��Ϸ���жϿɲ����Կ�ʼ������Ǳ�������ֱ�ӿ�ʼ��Ϸ
	{
		if(c1->guest)//���c1���Ƿ�������Ȩ������Ϸ
			return;
		if(uno_room[c1->room_No].room_member<=1)//���������ֻ��һ���ˣ���������������
		{
			send_msg_signal(c1,UNO_lack_of_players);
			return;
		}
	}//�жϽ�����֮����ȷ����Ϸ��ʼ

	UNO_room* this_room=&uno_room[c1->room_No];
	UNO_player* p1=NULL;//��ȡc1�û��Ľ�ɫp1
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
		this_room->game_over=false;//��Ϸ��û�н���
		UNO_send_game_start_signal(this_room);//���������Ϸ��ʼ��
		UNO_game_ini(this_room);
		UNO_decide_first_whose_turn(this_room);//ȷ��������˭�Ļغ�
		UNO_flop(this_room);//���ƣ���Ч��ʩ�Ӹ������ߣ������ĺ�Ŀ�����Ϣ�㲥

		//׼�����ˣ�֪ͨ������
		this_room->game_start=true;//��Ϸ��ʼ��
		Sleep(200);//��֤���л����˶����ٵȴ���Ϸ��ʶ����ȷ�������ܽ�����Ϸ

	}
	else//�������������Ϸ�Ļ��ˣ��ȴ���Ϸ��ʼ��AI�����߳���Ҳ�ȴ���Ϸ��ʼ
		while(!this_room->game_start)
			Sleep(200);
	

	while(true)
	{
		if(this_room->game_over)return;//�����Ϸ�����ˣ��ص�ԭ������
		
		memset(StrBuf,0,sizeof(StrBuf));
		addtext(StrBuf,UNO_get_client_message(p1,UNO_user_choose_time_limit));
		if(this_room->game_over){return;}//�����Ϸ�����ˣ��ص�ԭ������

		if(p1->card_color[1]==UNO_none&&p1->not_declare_UNO_time>=UNO_declare_time_limit)//û���޶�ʱ���ں�UNO
		{
			UNO_lock_player(this_room,p1);//�¼������������������߳�
			UNO_punish_for_not_declare_UNO(this_room,p1);//�ӷ�����
			UNO_release_player(this_room,p1);//�¼������������������߳�
			continue;
		}
		
		if(p1->think_time>=UNO_user_choose_time_limit)//����û�������˼��ʱ����û����ѡ��ǿ�Ƴ�1����
		{
			UNO_game_add_log(this_room,p1,"˼����ʱ!ǿ�����Ʋ������غ�!");
			UNO_user_apply_get_card(this_room,p1,1);//������ж�������мӷ��Ͳ��ó鿨��ֱ�����û�ѡ�񡰹������������ĺ���������Զ�ִ�мӷ�
			UNO_user_apply_change_turn(this_room,p1);//�ӷ�ֱ����������ִ��
			continue;
		}

		if(c1->F_offline)//���տͻ��˷�����Ϣ�������ʱ�Ͽ�ͨѶ������ͨ
		{
			UNO_lock_player(this_room,p1);//�����¼���������ס����ж�

			UNO_game_add_log(this_room,p1,"�˳�����Ϸ!");
			int human_number=0;//ͳ�Ʒ����������
				for(int i=0;i<UNO_member_limit;i++)
				{
					if(this_room->player[i].state==UNO_human)
						human_number++;
				}

			if(human_number==1)//��������ֻ����һ�����ˣ������ֵ����ˣ��򷿼���ֻʣ�������ˣ�ֱ���˳���Ϸ������֮�������������������溯��ִ���˷����ߵȲ���
			{
				//������Ϸ
			}
			else//���˵�����ﻹ�б�Ļ���
			{
				int player_number=0;//ͳ�Ʒ�����Ϸ���������������ˣ�
				for(int i=0;i<UNO_member_limit;i++)
				{
					if(this_room->player[i].state!=UNO_empty)
						player_number++;
				}
				
				if(player_number<=2)//���������ֻ����һ�������ˣ�û�����ˣ��������Ϸ������л������������Ϸ���Լ�ֱ��return�������κ���Ϸ����
					UNO_game_over_process(this_room,p1,false);
				else//������ˣ���ֻ�л��Լ��Ļغϣ��Ҷ����ֿ�
				{
					for(int i=0;i<UNO_deck_card_limit;i++)//�����ֿ�ȫ����Ĺ��
						if(p1->card_color[i]!=UNO_none)
						{
							UNO_add_card_to_tomb(this_room,p1->card_color[i],p1->card_effect[i]);
							p1->card_color[i]=UNO_none;
							p1->card_effect[i]=UNO_none;
						}
						else
							break;
					if(this_room->whose_turn==p1->room_member_NO)//�����ǰ���ҳ��ƵĻغ϶��ҵ�����
					{
						UNO_sys_change_turn(this_room);//�л��غ�
					}
				}
			}
			
			UNO_release_player(this_room,p1);//�����¼�����
			
				
			return;
		}

		if(this_room->game_over)return;//�����Ϸ�����ˣ��ص�ԭ������


		JSON_package json_msg(StrBuf);
		int choice=atoi(json_msg.get_value("signal"));
		switch(choice)
		{
			case UNO_apply_get_card://����鿨
				UNO_user_apply_get_card(this_room,p1,1);//������ж�������мӷ��Ͳ��ó鿨��ֱ�����û�ѡ�񡰹������������ĺ���������Զ�ִ�мӷ�
				
				break;
			case UNO_i_only_got_one_card:
				if(p1->card_color[1]!=UNO_none)
				{
					UNO_update_all_info_to_himself(this_room,p1);//����������Ϣ
					

					//cout<<"�������������ֿ�����������\n";
					break;
				}
				UNO_declare_UNO(this_room,p1);
				//�㲥uno��Ϣ
				break;
			case UNO_apply_pass://���롰����
				UNO_user_apply_change_turn(this_room,p1);//�ӷ�ֱ����������ִ��
				
				break;
			case UNO_apply_play_this_card://����
				//cout<<"����ѡ�����\n";
				UNO_play_this_card(this_room,p1,atoi(json_msg.get_value("which_card")));//���ƣ��ú����ڰ����ж��ܲ��ܳ���������ƣ��ú������ִ�лغ��л�
				break;
			
			default:
				cout<<"�������\n";
				break;
			
		}
			

	}

	

	

}

#endif