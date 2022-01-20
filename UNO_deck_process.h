#ifndef INCLUDE_UNO_deck_process_H
#define INCLUDE_UNO_deck_process_H

#include "UNO_room_process.h"

void UNO_lock_player(UNO_room* this_room,UNO_player* p1)//�������������û��̣߳�һ���ڳɹ��ܴ��һ����ʱʹ��
{
	if(this_room->whether_lock)return;//����Ѿ����ˣ���ִ������
	this_room->whether_lock=true;
	for(int i=0;i<UNO_member_limit;i++)
	{
		switch(this_room->player[i].state)
		{
			case UNO_human:
				if(this_room->player[i].room_member_NO!=p1->room_member_NO)//�����ǰλ�����˲��Ƿ����¼���
					SuspendThread(hthread[this_room->player[i].c1->member_No-1]);//��ס���ļ����߳�
				break;
			case UNO_empty:
				break;
			default://������
				if(this_room->player[i].room_member_NO!=p1->room_member_NO)//�����ǰλ�����˲��Ƿ����¼���
					SuspendThread(this_room->AI_thread[i]);//��ס���ļ����߳�
		}
	}
	Sleep(200);
}
void UNO_release_player(UNO_room* this_room,UNO_player* p1)//��ŷ��������û��߳�
{
	if(!this_room->whether_lock)return;//���û������ִ�н���
	this_room->whether_lock=false;

	for(int i=0;i<UNO_member_limit;i++)
	{
		switch(this_room->player[i].state)
		{
			case UNO_human:
				if(this_room->player[i].room_member_NO!=p1->room_member_NO)//�����ǰλ�����˲��Ƿ����¼���
					ResumeThread(hthread[this_room->player[i].c1->member_No-1]);//������ļ����߳�
				
				break;
			case UNO_empty:
				break;
			default://������
				if(this_room->player[i].room_member_NO!=p1->room_member_NO)//�����ǰλ�����˲��Ƿ����¼���
					ResumeThread(this_room->AI_thread[i]);//������ļ����߳�
		}
	}
}
void UNO_clean_deck(UNO_room* this_room)//����ȫ���ÿ�
{
	for(int i=0;i<UNO_deck_card_limit;i++)
	{
		this_room->deck_card_color[i]=UNO_none;
		this_room->deck_card_effect[i]=UNO_none;
		this_room->tomb_card_color[i]=UNO_none;
		this_room->tomb_card_color[i]=UNO_none;
	}
}
void UNO_add_card_to_tomb(UNO_room* this_room,int color,int effect )//���ÿ�����Ĺ��
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
void UNO_add_card_to_deck(UNO_room* this_room,int color,int effect )//���ÿ����뿨��
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

void UNO_deck_card_all_into_tomb(UNO_room* this_room)//�ѿ�����Ŀ�ȫ�ص�Ĺ��
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
	//��ȷ��name����ʼ����={0}
	if(p1!=NULL)
		switch(p1->state)
		{
			case UNO_empty:
				addtext(name,"��");
				break;
			case UNO_human:
				addtext(name,p1->room_member_NO+1);
				addtext(name,"����� ");
				addtext(name,p1->c1->client_name);
				addtext(name," ");
				break;
			case UNO_hard_AI:
				addtext(name,"һ�������");
				addtext(name,p1->room_member_NO+1);
				addtext(name,"��");
				addtext(name," ");
				break;
			case UNO_simple_AI:
				addtext(name,"�򵥻�����");
				addtext(name,p1->room_member_NO+1);
				addtext(name,"��");
				addtext(name," ");
				break;
			default:addtext(name,"��ֶ���");
		}
	else
		return "";
	return name;
}

void UNO_game_add_log(UNO_room* this_room,UNO_player* p1,char* StrBuf)//������Ϸ��Ϣ��־
{
	//û��UNO
	//�û�ѡ����ɫ��ʱ
	//�û�ѡ����Ƴ�ʱ
	//ѡ�񡰹���
	//ѡ������
	//�����ɫ�ƺ�ѡ����ɫ(���յ���ɫ������Ϣ��ͻ����Զ����log)
	//��ֹ�ƣ�˭�������ˣ�
	//�غ���ת
	//˭���ܼӷ�
	//���һ����/����
	//�����ˡ�����
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


int check_how_many_card_in_deck(UNO_room* this_room)//��鿨����ٿ�
{
	int deck_card_number=0;
	while(this_room->deck_card_color[deck_card_number]!=UNO_none)
	{
		deck_card_number++;
		if(deck_card_number==UNO_deck_card_limit)//����Ѿ����￨�龡ͷ��˵����������
			break;
	}
	return deck_card_number;
}
int check_how_many_card_in_tomb(UNO_room* this_room)//���Ĺ�ض��ٿ�
{
	int tomb_card_number=0;
	while(this_room->tomb_card_color[tomb_card_number]!=UNO_none)
	{
		tomb_card_number++;
		if(tomb_card_number==UNO_deck_card_limit)//����Ѿ����￨�龡ͷ��˵����������
			break;
	}
	return tomb_card_number;
}
int check_how_many_card_in_player(UNO_player* p1)//�����Ҷ����ֿ�
{
	int player_card_number=0;
	while(p1->card_color[player_card_number]!=UNO_none)
	{
		player_card_number++;
		if(player_card_number==UNO_deck_card_limit)//����Ѿ����￨�龡ͷ��˵���ֿ�����
		{
			cout<<"�ֿ���������\n";
			break;
		}
	}
	return player_card_number;
}

char* UNO_get_this_card_color(int color)
{
			switch(color)
			{
				case UNO_green:return "��ɫ";
				case UNO_red:return "��ɫ";
				case UNO_blue:return "��ɫ";
				case UNO_yellow:return "��ɫ";
				case UNO_black:return "��ɫ";
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
				case UNO_reverse:	return "��ת��";
				case UNO_forbid:	return "�谭��";
				case UNO_ADD_2:		return "+2";break;
				case UNO_universal:	return "������";
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
void UNO_show_deck(UNO_room* this_room)//չʾ��ǰ���俨�����п�
{
	char content[200]={0};
	cout<<"�����ڿ�:\n";
	for(int kk=0;kk<UNO_deck_card_limit;kk++)
	{
		UNO_cout_this_card(this_room->deck_card_color[kk],this_room->deck_card_effect[kk]);
		cout<<endl;
	}
}
void UNO_update_all_info_to_himself(UNO_room* this_room,UNO_player* p1)//����������һ����
{
	if(p1->state!=UNO_human)return;
	JSON_package json_msg;
	json_msg.add_item("signal",UNO_refresh_all_data);
	json_msg.add_item("last_color",this_room->last_color);
	json_msg.add_item("last_effect",this_room->last_effect);
	json_msg.add_item("current_color",this_room->current_color);
	json_msg.add_item("whose_turn",this_room->whose_turn);//˭�Ļغ�
	if(this_room->clock_direct)//˳ʱ����ʱ��
		json_msg.add_item("clock_direct",UNO_clockwise);
	else
		json_msg.add_item("clock_direct",UNO_anti_clockwise);

	JSON_package card_number;
	for(int i=0;i<UNO_member_limit;i++)
		card_number.add_item_array("card_number",check_how_many_card_in_player(&this_room->player[i]));//�����ֿ�����
	json_msg.add_struct("card_number",card_number.get_json());
	//cout<<"�����:"<<check_how_many_card_in_player(p1)<<endl;

	//�Լ��ֿ���������
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
	if(p1->card_color[0]!=UNO_none)//�����߻Ὣ�����ƷŻ�Ĺ�أ��Ӷ��������û���ֿ�
	{
		json_msg.add_struct("my_card_color",card_color.get_json());//���Լ��ֿ���Ϣȫ����ṹ��
		json_msg.add_struct("my_card_effect",card_effect.get_json());
	}
	send_msg(p1->c1,json_msg.to_StrBuf());
}


void UNO_broadcast_all_info(UNO_room* this_room)//ȫԱˢ����Ϣ���л��غϺ�ʹ�ã�ĳ�˷Ǽӷ��鿨��ʹ��
{
	for(int i=0;i<UNO_member_limit;i++)
		if(this_room->player[i].state==UNO_human)
			UNO_update_all_info_to_himself(this_room,&this_room->player[i]);//����������Ϣ	
}


void UNO_sys_change_turn(UNO_room* this_room)//��һ���˵Ļغ��ˣ����ý�ɫ�ǻ����ˣ�ִ��AI˼��
{
	//�ӷ�������Ч������ִ��Ч����ĸú��������
	for(int i=0;i<UNO_member_limit;i++)
		this_room->player[i].whether_get_card=false;//����������״̬��Ϊ��û������
	
	if(this_room->clock_direct)//˳ʱ��
	{
		while(true)
		{
			this_room->whose_turn++;
			if(this_room->whose_turn==UNO_member_limit)
					this_room->whose_turn=0;
			while(this_room->player[this_room->whose_turn].state==UNO_empty)//����ǰλ����Ϊ�գ���һ��
			{
				this_room->whose_turn++;
				if(this_room->whose_turn==UNO_member_limit)
					this_room->whose_turn=0;
			}
			//ִ�е������Ȼ�ҵ���Ϊ�յ�λ����
			if(this_room->current_running_effect==UNO_forbid)//����н�ֹЧ������������һغ�,����Ѱ���¸���Ҳ�����Ϊ���Ļغ�
			{
				UNO_game_add_log(this_room,&this_room->player[this_room->whose_turn],"��������!");
				this_room->current_running_effect=UNO_none;//��ս�ֹЧ��
			}
			else//���û�н�ֹЧ�����Ƴ���ѭ��
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
			while(this_room->player[this_room->whose_turn].state==UNO_empty)//����ǰλ����Ϊ�գ���һ��
			{
				this_room->whose_turn--;
				if(this_room->whose_turn<0)
					this_room->whose_turn=UNO_member_limit-1;
			}
			//ִ�е������Ȼ�ҵ���Ϊ�յ�λ����
			if(this_room->current_running_effect==UNO_forbid)//����н�ֹЧ������������һغ�,����Ѱ���¸���Ҳ�����Ϊ���Ļغ�
			{
				UNO_game_add_log(this_room,&this_room->player[this_room->whose_turn],"��������!");
				this_room->current_running_effect=UNO_none;//��ս�ֹЧ��
			}
			else//���û�н�ֹЧ�����Ƴ���ѭ��
				break;
		}
	}
	UNO_broadcast_all_info(this_room);
	
}



void UNO_refresh_player_card_add(UNO_player* p1,bool whether_refresh_data)//��Ϊ������ֿ����������ֿ�����������
{
	for(int i=0;i<UNO_deck_card_limit;i++)
	{
		if(p1->card_color[i]==UNO_none)
			break;
		for(int j=0;j<UNO_deck_card_limit;j++)//����ɫ���ȼ���͵ġ������ǰ�����ǰ��
		{
			if(p1->card_color[j]==UNO_none)
				break;

			if(p1->card_color[i]>p1->card_color[j])//�����ɫ��ͬ�����ȼ��͵Ļ���ǰ��
			{
				int temp_color=p1->card_color[i];
				int temp_effect=p1->card_effect[i];
				p1->card_color[i]=p1->card_color[j];
				p1->card_effect[i]=p1->card_effect[j];
				p1->card_color[j]=temp_color;
				p1->card_effect[j]=temp_effect;
			}
			if(p1->card_color[i]==p1->card_color[j]&&p1->card_effect[i]>p1->card_effect[j])//�����ɫ��ͬ�����С�Ļ���ǰ��
			{
				int temp_effect=p1->card_effect[i];
				p1->card_effect[i]=p1->card_effect[j];
				p1->card_effect[j]=temp_effect;
			}
		}
	}
	//�����ûUNO�ӷ���Ҫ��������
	//���˵�Ǽӷ������ø������ݣ��ӷ�����Զ��غ��л����ڻغ��л�����ִ�и�������
	//���˵���״������˳鿨�����ڷ���ʱ�������������ݣ��״������˳鿨�൱�ڼӷ�
	//������Լ�ѡ�����ƣ��������������������
	if(whether_refresh_data)
		UNO_broadcast_all_info(&uno_room[p1->room_No]);
	
}

void UNO_refresh_player_card_used(UNO_player* p1)//��Ϊʹ�����ֿ����������ֿ��������������������
{
	int i;
	for(i=0;i<UNO_deck_card_limit;i++)
	{
		if(p1->card_color[i]==UNO_none)
			break;
	}
	if(i==UNO_deck_card_limit-1||p1->card_color[i+1]==UNO_none)//�պ��õ������е����һ�ſ�,��������
	{
		//�����ƺ��Զ��л��غϣ���������и���ȫԱ����
		return;
	}

	while(true)//�������Ǹ����ŵ�ǰ����
	{
		p1->card_color[i]=p1->card_color[i+1];
		p1->card_effect[i]=p1->card_effect[i+1];
		i++;
		if(i==UNO_deck_card_limit-1||p1->card_color[i+1]==UNO_none)//���˵�ֿ��۵�ͷ�ˣ���ǰ���п����¸����޿�����֮ǰ�����Ѿ����Ƶ�ǰ���Ǹ����ˣ����Ըò��ÿղ���������
		{
			p1->card_color[i]=UNO_none;
			p1->card_effect[i]=UNO_none;
			break;
		}
			
	}
	//�����ƺ���Զ��л��غϣ���������и���ͨѶ�������ֿ�
}


void UNO_shuffle(UNO_room* this_room)//ϴ�ƣ������鿨���ж�û���󡿡�����ʼ�������ʹ�øú�������tombĹ������Ʒ���deck
{
	UNO_game_add_log(this_room,NULL,"ϵͳ��ʼϴ��");
	int card_number=check_how_many_card_in_tomb(this_room);//ȷ��Ĺ����Ŀ�Ƭ����
	
	if(check_how_many_card_in_deck(this_room)>0)
	{
		//cout<<"�����￨��Ϊ0������ϴ�ƣ�\n";
		return;
	}
	
	if(card_number==0)//Ĺ���￨Ϊ0���޷�ϴ��
	{
		//cout<<"Ĺ���￨Ϊ0������ϴ�ƣ�\n";
		return;
	}

	int tomb_card_number=card_number;//ȷ�����˿�Ƭ����
	int result=0;
	while(true)
	{
		srand((unsigned)time(NULL)*rand()); //���ȡֵ0-card_number-1
		
		result=rand()%(card_number-1-0);
		
		while(this_room->tomb_card_color[result]==UNO_none)//���Ϊ�գ�����һ����������û����
		{
			result--;
			if(result==-1)
				result=card_number-1;
		}//ȷ�������ˣ����Ʒ��뿨��
		UNO_add_card_to_deck(this_room,this_room->tomb_card_color[result],this_room->tomb_card_effect[result]);
		this_room->tomb_card_color[result]=UNO_none;this_room->tomb_card_effect[result]=UNO_none;//�ÿ����ÿ�
		tomb_card_number--;
		if(tomb_card_number==0)break;//��Ĺ����û���ˣ�����ϴ��
	}

}




void UNO_ini_deck(UNO_room* this_room)//��ʼ�����飬ֻ����Ϸ��ʼʱ�õ����Ƚ���������Ĺ�أ���ϴ�Ʒ�ʽ���ؿ���
{
	//���н�ɫ���п������start_game��ʹ��

	//����Ĺ�ؿ����
	UNO_clean_deck(this_room);

	//��Ĺ���������п�Ƭ
	//0������
	UNO_add_card_to_tomb(this_room,UNO_green,0);
	UNO_add_card_to_tomb(this_room,UNO_red,0);
	UNO_add_card_to_tomb(this_room,UNO_blue,0);
	UNO_add_card_to_tomb(this_room,UNO_yellow,0);

	//1-9��ÿ������
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
	
	//+2��ÿ����ɫ����
	UNO_add_card_to_tomb(this_room,UNO_green,UNO_ADD_2);
	UNO_add_card_to_tomb(this_room,UNO_green,UNO_ADD_2);
	UNO_add_card_to_tomb(this_room,UNO_red,UNO_ADD_2);
	UNO_add_card_to_tomb(this_room,UNO_red,UNO_ADD_2);
	UNO_add_card_to_tomb(this_room,UNO_blue,UNO_ADD_2);
	UNO_add_card_to_tomb(this_room,UNO_blue,UNO_ADD_2);
	UNO_add_card_to_tomb(this_room,UNO_yellow,UNO_ADD_2);
	UNO_add_card_to_tomb(this_room,UNO_yellow,UNO_ADD_2);

	//��ת�ƣ�ÿ������
	UNO_add_card_to_tomb(this_room,UNO_green,UNO_reverse);
	UNO_add_card_to_tomb(this_room,UNO_green,UNO_reverse);
	UNO_add_card_to_tomb(this_room,UNO_red,UNO_reverse);
	UNO_add_card_to_tomb(this_room,UNO_red,UNO_reverse);
	UNO_add_card_to_tomb(this_room,UNO_blue,UNO_reverse);
	UNO_add_card_to_tomb(this_room,UNO_blue,UNO_reverse);
	UNO_add_card_to_tomb(this_room,UNO_yellow,UNO_reverse);
	UNO_add_card_to_tomb(this_room,UNO_yellow,UNO_reverse);

	//�赲�ƣ�ÿ������
	UNO_add_card_to_tomb(this_room,UNO_green,UNO_forbid);
	UNO_add_card_to_tomb(this_room,UNO_green,UNO_forbid);
	UNO_add_card_to_tomb(this_room,UNO_red,UNO_forbid);
	UNO_add_card_to_tomb(this_room,UNO_red,UNO_forbid);
	UNO_add_card_to_tomb(this_room,UNO_blue,UNO_forbid);
	UNO_add_card_to_tomb(this_room,UNO_blue,UNO_forbid);
	UNO_add_card_to_tomb(this_room,UNO_yellow,UNO_forbid);
	UNO_add_card_to_tomb(this_room,UNO_yellow,UNO_forbid);

	//��ɫ������4��
	UNO_add_card_to_tomb(this_room,UNO_black,UNO_universal);
	UNO_add_card_to_tomb(this_room,UNO_black,UNO_universal);
	UNO_add_card_to_tomb(this_room,UNO_black,UNO_universal);
	UNO_add_card_to_tomb(this_room,UNO_black,UNO_universal);

	//��ɫ+4��4��
	UNO_add_card_to_tomb(this_room,UNO_black,UNO_ADD_4);
	UNO_add_card_to_tomb(this_room,UNO_black,UNO_ADD_4);
	UNO_add_card_to_tomb(this_room,UNO_black,UNO_ADD_4);
	UNO_add_card_to_tomb(this_room,UNO_black,UNO_ADD_4);

	//ϴ�ƣ���Ĺ����������ϴ��������,ϴ����
	UNO_shuffle(this_room);
	UNO_deck_card_all_into_tomb(this_room);
	UNO_shuffle(this_room);
	UNO_deck_card_all_into_tomb(this_room);
	UNO_shuffle(this_room);
	
	//UNO_show_deck(this_room);//��ʾ���������п�

}



void UNO_update_current_color(UNO_room* this_room,UNO_player* p1,int color)//�����û������˿��Գ��Ƶ���ɫ
{
	this_room->current_color=color;
	JSON_package json_msg;
	json_msg.add_item("signal",UNO_player_choose_color_change);
	json_msg.add_item("current_color",this_room->current_color);
	char name[200]={0};
	if(p1!=NULL)
		json_msg.add_item("event_name",UNO_get_player_name(name,p1));
	for(int i=0;i<UNO_member_limit;i++)//֪ͨ���������л��˸��µ�ǰ���Գ����Ƶ���ɫ
		if(this_room->player[i].state==UNO_human)
			send_msg(this_room->player[i].c1,json_msg.to_StrBuf());
}
void UNO_update_current_direct(UNO_room* this_room)//�����û�������˳������ʱ��˳ʱ��,�ڷ��ƺʹ��������ʱ��ʹ��
{
	if(this_room->clock_direct)
		this_room->clock_direct=false;
	else
		this_room->clock_direct=true;

	char log_content[300]={0};
	addtext(log_content,"��ǰ˳����ı���!��Ϊ");
	if(this_room->clock_direct)
		addtext(log_content,"˳ʱ��!");
	else
		addtext(log_content,"��ʱ��!");

	UNO_game_add_log(this_room,NULL,log_content);
}


void UNO_update_current_card(UNO_room* this_room,UNO_player* p1,int color,int effect)//����������һ����
{
	this_room->last_color=color;
	this_room->last_effect=effect;
	if(this_room->last_color!=UNO_black)//�������Ϊ���ƣ�����֮ǰ����������������ɫ������last_color���˴����踳ֵ������ͽ��������һ���Ƶ���ɫ����Ϊ����ɫ
		this_room->current_color=this_room->last_color;
	if(p1!=NULL)
	{
		char log_content[300]={0};
		addtext(log_content,"���:");
		addtext(log_content,UNO_get_this_card_color(color));
		addtext(log_content,UNO_get_this_card_effect(effect));
		UNO_game_add_log(this_room,p1,log_content);//˭���ʲô��
		return;//�ڴ���ƺ�ִ�еĻغ��л���ִ��ȫ������
	}
	//ִ�е�����˵�����״η���

	UNO_broadcast_all_info(this_room);//����������Ϣ
}




void UNO_declare_UNO(UNO_room* this_room,UNO_player* p1)
{
	if(p1->whether_declare_UNO)return;//�Ѿ���������
	if(p1->card_color[1]!=UNO_none)
		return;
	p1->whether_declare_UNO=true;
	UNO_game_add_log(this_room,p1,"UNO!��ֻʣ���һ��������!");
}


void UNO_game_over_process(UNO_room* this_room,UNO_player* p1,bool win)//p1Ӯ�ˣ�����Ϸ���������ˣ�ֻʣ��������Ϸʱ���뿪һ���˺��Ҳ�ʣ������ʱ����������Ϸ����
{

	this_room->game_over=true;
	this_room->game_start=false;

	JSON_package json_msg;
	UNO_add_room_message(this_room,&json_msg);//��ӷ����Ա��Ϣ

	for(int i=0;i<UNO_member_limit;i++)
	{
		if(this_room->player[i].state==UNO_human)
		{
			json_msg.add_item("your_number",i);
			this_room->player[i].c1->state=UNO_in_room;//״̬����ڷ�����
			if(win)//����ǻ�ʤ����������Ϸ
			{
				json_msg.add_item("winner_number",p1->room_member_NO);
				json_msg.add_item("signal",UNO_game_over_someone_win);//��ʤ
				char name[200]={0};
				json_msg.add_item("winner",UNO_get_player_name(name,p1));
				send_msg(this_room->player[i].c1,json_msg.to_StrBuf());				
			}
			else//����������������������Ϸ
			{
				json_msg.add_item("signal",UNO_game_over_lack_player);
				send_msg(this_room->player[i].c1,json_msg.to_StrBuf());	
			}

		}
	}
}


bool UNO_judge_whether_can_play(UNO_room* this_room,UNO_player* p1,int choice)//�ж��Ƿ��ܴ��������
{
	if(this_room->current_running_effect==UNO_add_card)//������ǰ�Ƿ��мӷ�Ч��
		switch(this_room->last_effect)//�鿴���һ�����õĿ�������+2��+4����������+4���2
		{
			case UNO_ADD_2://+2�����ܼ�+4��+2
				if(p1->card_effect[choice]==UNO_ADD_4||p1->card_effect[choice]==UNO_ADD_2)
					return true;
				else
					return false;
			case UNO_ADD_4://+4����ֻ��+4
				if(p1->card_effect[choice]==UNO_ADD_4)
					return true;
				else
					return false;
		}

	//ִ�е������Ȼûʩ�Ӽӿ���Ч��
	
	if(p1->card_color[choice]==UNO_black)//���ƿ���ֱ�ӳ�
		return true;

	if(p1->card_color[choice]!=this_room->current_color&&p1->card_effect[choice]!=this_room->last_effect)//���ѡ�е��ƼȲ�ͬɫ�ֲ�ͬ��/ͬЧ�������ܴ��
		return false;//�޷�ʹ��

	return true;
}

void UNO_seize_turn(UNO_room* this_room,UNO_player* p1)//�����ͬ����
{
	if(this_room->whose_turn!=p1->room_member_NO)//�����ǰ�����ҵĻغ�
	{
		this_room->whose_turn=p1->room_member_NO;//ǿ�Ʊ�Ϊ�ҵĻغ�	
		UNO_game_add_log(this_room,p1,"�������ͬ���ƣ���ռ�˸ûغ�!");
	}
}

void UNO_player_choose_color(UNO_room* this_room,UNO_player* p1);
void UNO_play_this_card(UNO_room* this_room,UNO_player* p1,int choice)//���������
{

	if(choice<0||choice>UNO_deck_card_limit)//����ָ��λ��Խ�磬��������
	{
		//cout<<"����ָ��λ��Խ�磬��������\n";
		return;
	}
	if(p1->card_color[choice]==UNO_none)//�ò���û�п�
	{
		//cout<<choice<<"�ò���û�п�!\n";		
		UNO_update_all_info_to_himself(this_room,p1);//����������Ϣ
		return;
	}

	//�ж������ܲ���ʹ��
	if(!UNO_judge_whether_can_play(this_room,p1,choice))//������ܴ�������ƣ�������ʾ
	{
		if(p1->state!=UNO_human){cout<<"�����˴������!";UNO_cout_this_card(p1->card_color[choice],p1->card_effect[choice]);return;}
		UNO_update_all_info_to_himself(this_room,p1);//����������Ϣ
		if(this_room->whose_turn==p1->room_member_NO)//�����ǰ���Լ��غ�
		{
			if(this_room->current_running_effect==UNO_add_card)//�����ǰ�мӷ�Ч��
				UNO_tips(p1,UNO_got_punish);
			else
				UNO_tips(p1,UNO_cannot_play_this_card);//������ʾ���޷����������
		}
		//cout<<"���һ������";cout_this_card(this_room->last_color,this_room->last_effect);
		//cout<<"�ɳ�����ɫ��Ч����:";cout_this_card(this_room->current_color,this_room->last_effect);
		//cout<<"���������:";cout_this_card(p1->card_color[choice],p1->card_effect[choice]);
		//cout<<"����ʧ��\n";
		return ;//�޷�ʹ��
	}
	//ִ�е������Ȼ�ܴ�����ſ�����Ҫ�ж��Ƿ����Լ��غϣ��ǵ�����ԣ������򲻿���

	if(p1->card_color[choice]==this_room->last_color&&p1->card_effect[choice]==this_room->last_effect)//�������ӵ�ǰ�Ƿ����ҵĻغϣ�����������뵱ǰ��һ�£�ֱ�ӳ�
	{
		UNO_seize_turn(this_room,p1);//ǿ�Ʊ�Ϊ�ҵĻغ�,��������ſ�
	}
	else
		if(this_room->whose_turn!=p1->room_member_NO)//����ǰ�����ҵĻغ�
		{
			UNO_update_all_info_to_himself(this_room,p1);//����������Ϣ
			//cout<<"��ǰ�����ҵĻغϣ���������,�޷�����\n";
			return;
		}

	//ִ�е������Ȼ�ܴ��������
	//���������û��̣߳����������˵ģ��������������Ƶ��¼�
	UNO_lock_player(this_room,p1);

	//���µ�ǰ˭�����һ����
	UNO_update_current_card(this_room,p1,p1->card_color[choice],p1->card_effect[choice]);

	int color=p1->card_color[choice];
	int effect=p1->card_effect[choice];

	//����ÿ���Ч��������Ч����Ч���ڷ�������轫current_running_effect��������ΪUNO_none
	switch(effect)
	{
		case UNO_ADD_2://�ӷ�2
			this_room->current_running_effect=UNO_add_card;//��ǰЧ������Ϊ�ӷ���Ƭ
			this_room->punish_card_number+=2;//���üӷ�����
			break;
		case UNO_ADD_4://�ӷ�4
			this_room->current_running_effect=UNO_add_card;//��ǰЧ������Ϊ�ӷ���Ƭ
			this_room->punish_card_number+=4;//���üӷ�����
			break;
		case UNO_reverse://��ת
			UNO_update_current_direct(this_room);
			break;
		case UNO_forbid:
			this_room->current_running_effect=UNO_forbid;//�����л��غ�ʱ��飬���иñ�־���������¸��˵Ļغ�
			break;
	}

	//��������Ĺ��
	UNO_add_card_to_tomb(this_room,color,effect);

	//��ոÿ���
	p1->card_color[choice]=UNO_none;
	p1->card_effect[choice]=UNO_none;

	UNO_refresh_player_card_used(p1);//��Ϊ�����һ�ſ������������ֿ�

	if(color==UNO_black)//���˵�Ǻڿ������ѡ����ɫ
		UNO_player_choose_color(this_room,p1);//this_room->current_color=choice;

	

	//����ǻ����ˣ�����ֻʣһ���ƣ�������Զ���uno
	if(p1->card_color[1]==UNO_none&&p1->state!=UNO_human)
		UNO_declare_UNO(this_room,p1);

	//���Ӯ�ˣ�֪ͨ��������Ϸ����
	if(p1->card_color[0]==UNO_none)
		UNO_game_over_process(this_room,p1,true);//cout<<"�����ֿ�������!\n";
	else
		UNO_sys_change_turn(this_room);//���һ���ƣ���Ȼ�л���ɫ�غ�

	//�����¼���ɣ���������û��̼߳���
	UNO_release_player(this_room,p1);
	return ;

}

void UNO_get_card(UNO_room* this_room,UNO_player* p1,int number,bool whether_refresh_data)//����ֿ��������鿨��ӷ�
{
	if(number<=0){cout<<"�鿨����Ϊ0��\n";return;}

	p1->whether_get_card=true;//ȷ���Ѿ�������

	int deck_card_number=check_how_many_card_in_deck(this_room);//ȷ�Ͽ������ж��ٿ�
	if(deck_card_number==0)//�������û��,ȷ��Ĺ������û�п������û�У����ز��ó鿨
	{
		int tomb_card_number=check_how_many_card_in_tomb(this_room);
		if(tomb_card_number==0)
			return;//˵��Ĺ����Ҳû�п������ز��ó鿨
		else
		{
			UNO_shuffle(this_room);//˵��Ĺ�����п�����Ĺ���п���ϴ������
			deck_card_number=check_how_many_card_in_deck(this_room);//�ٴ�ȷ�Ͽ����ж��ٿ�
		}
	}

	//���˴��������Ȼ�п�����Ȼ�ܳ鿨
	p1->whether_declare_UNO=false;//����֮ǰ��û�к���UNO��ʧЧ
	deck_card_number--;//deck_card_numberָ�����һ���ǿյĿ���

	//ȷ������ж����ֿ�	
	int player_card_number=check_how_many_card_in_player(p1);
	//player_card_number��ʱ���ָ���λ

	while(number>0)//����Ҫ�鿨ʱ
	{
		p1->card_color[player_card_number]=this_room->deck_card_color[deck_card_number];
		p1->card_effect[player_card_number]=this_room->deck_card_effect[deck_card_number];
		this_room->deck_card_color[deck_card_number]=UNO_none;//������ԭ�������
		this_room->deck_card_effect[deck_card_number]=UNO_none;//������ԭ�������
		player_card_number++;//���ӵ�п�����+1��ָ����һ���ղ۽������
		number--;//��Ҫ�鿨������-1
		deck_card_number--;//����ָ�����һ���ǿտ���
		if(deck_card_number==-1)//�������û����
		{
			int tomb_card_number=check_how_many_card_in_tomb(this_room);//ȷ��Ĺ�����ж��ٿ�
			if(tomb_card_number==0)break;//���Ĺ��Ҳû���ˣ�����������
			UNO_shuffle(this_room);//ȷ��Ĺ�ػ��п���ϴ�ƣ���Ĺ���п����뿨��
		}
	}

	//���˵�Ǽӷ������ø������ݣ��ӷ����Զ��ڻغ��л���ִ�и�������
	//���˵���״������˳鿨������Ϊ�ӷ���֮���ڷ���ʱ��������������
	//������Լ�ѡ�����ƣ����������
	UNO_refresh_player_card_add(p1,whether_refresh_data);//��Ϊ������ֿ����������ֿ�����������ͬʱ֪ͨ�����и���
}

void UNO_user_accept_punish_card(UNO_room* this_room,UNO_player* p1,int number)//��ɫ���ܼӷ�
{
	if(number<=0){cout<<"�ӷ�0�ţ�\n";return;}

	char log[100]={0};addtext(log,"���ܼӷ�: ");addtext(log,number);addtext(log,"��!");
	UNO_game_add_log(this_room,p1,log);
	UNO_get_card(this_room,p1,number,false);//�鿨

	this_room->punish_card_number=0;//�ӷ�������Ϊ0
	this_room->current_running_effect=UNO_none;//�ӷ�Ч�����
}


void UNO_user_apply_get_card(UNO_room* this_room,UNO_player* p1,int number)//��ɫ�������ֿ�
{
	if(this_room->whose_turn!=p1->room_member_NO)//�����ǰ�����ҵĻغ�
	{
		return;//cout<<"�����ҵĻغϣ�ֱ����������\n";
	}

	if(this_room->current_running_effect==UNO_add_card)//�����ǰ�мӷ�Ч����û�ƿɳ������ó鿨��ֱ��ѡ�����������������ó鿨��
	{
		UNO_tips(p1,UNO_got_punish_choose_pass);//cout<<"��ǰ�мӷ�Ч�������ó鿨����ֱ��ѡ�񡰹���\n";
		return;
	}
	if(this_room->player[p1->room_member_NO].whether_get_card)//����Լ�������
	{
		UNO_tips(p1,UNO_got_card_please_choose_pass);//cout<<"�Ѿ������ƣ���ѡ��"��"\n";
		return;;
	}
	//ִ�е������Ȼ������
	UNO_game_add_log(this_room,p1,"ѡ������");	
	UNO_lock_player(this_room,p1);//�¼������������������߳�
	UNO_get_card(this_room,p1,number,true);
	UNO_release_player(this_room,p1);//�¼������������������߳�
}
void UNO_user_apply_change_turn(UNO_room* this_room,UNO_player* p1)//�û������л��غϣ�����мӷ�Ч�����ӷ��������л��غ�
{
	if(this_room->whose_turn!=p1->room_member_NO)
	{
		//cout<<"�����ҵĻغϣ�ֱ����������\n";
		return;
	}
	if(p1->state==UNO_human&&p1->c1->F_offline)//���˵�ǻ������غϵ���
		;
	else
		if(this_room->current_running_effect!=UNO_add_card)//����мӷ�Ч�������������ж�
			if(!this_room->player[p1->room_member_NO].whether_get_card)//�����û�����ƣ���������
			{
				UNO_tips(p1,UNO_please_get_card);//cout<<p1->c1->client_name<<"û�����أ�\n";
				return;
			}

	//ִ�е������Ȼ��ѡ��"��"
	UNO_lock_player(this_room,p1);
	UNO_game_add_log(this_room,p1,"ѡ��\"��\"");
	if(p1->state==UNO_human&&p1->c1->F_offline)//���˵�ǻ������غϵ���
		;
	else//������˵����мӷ�Ч����������һ���ˣ����򷣸������
		if(this_room->current_running_effect==UNO_add_card)//����мӷ�Ч��
		{
			UNO_user_accept_punish_card(this_room,p1,this_room->punish_card_number);//�ӷ�����
		}
	UNO_sys_change_turn(this_room);//�л��غ�
	UNO_release_player(this_room,p1);
}







#endif