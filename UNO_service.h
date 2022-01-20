#ifndef INCLUDE_UNO_service_H
#define INCLUDE_UNO_service_H

#include "UNO_start_game.h"

bool UNO_provide_service(client_member* c1,JSON_package json_msg)
{
	//char StrBuf[MAX_PATH] = { 0 };//��������洢���յ�������
	//char Buf[MAX_PATH] = { 0 };//ר������ת�����ַ���ʽ��ֵΪ�ַ�
	
	
	switch(atoi(json_msg.get_value("signal")))//�����ǰ��UNOѡ�����������״̬�����ϣ����д���
	{
		case UNO_apply_create_room:
		case UNO_apply_enter_this_room:
		case UNO_apply_enter_room:
		case UNO_apply_exit_roomlist://ѡ��ȡ�����뷿��
			if(c1->state==in_online)break;//���Խ��д������
			if(c1->state==UNO_in_room)//����Ѿ��ڷ���
			{	
				UNO_refresh_room_member_to_all(UNO_already_in_room,&uno_room[c1->room_No],c1);//ˢ�³�Ա
				return true;
			}
			if(c1->state!=in_online)//��ǰ״̬�޷������������!
			{send_msg_signal(c1,UNO_cannot_operate_this);return true;;}
			break;
		case UNO_apply_exit_room:
		case UNO_apply_add_full_hard_robot:
		case UNO_apply_start_game:
			if(c1->state==UNO_in_room)break;//���Խ��д������
			if(c1->state==in_online)
			{send_msg_signal(c1,UNO_leave_room_success);return true;}//ǿ���˳�����
			if(c1->state==UNO_in_game)
			{send_msg_signal(c1,UNO_game_start);return true;}
			if(c1->state!=UNO_in_room)
			{send_msg_signal(c1,UNO_not_got_room_operate_right);return true;}
			break;
	}

	switch(atoi(json_msg.get_value("signal")))
	{
		case UNO_apply_create_room:
			UNO_create_room(c1);
			return true;
		case UNO_apply_enter_this_room:
			UNO_enter_room(c1,json_msg);
			return true;
		case UNO_apply_enter_room:
			UNO_show_room_in_use(c1);//����չʾ�����ܼ���ķ���
			return true;
		case UNO_apply_exit_room:
			UNO_exit_room(c1);
			return true;
		case UNO_apply_add_full_hard_robot:
			UNO_add_FULL_robot_to_room(c1);
			return true;
		case UNO_apply_start_game:
			UNO_start_one_game(c1,true);
			return true;
		case UNO_apply_exit_roomlist://ѡ��ȡ�����뷿��
			send_msg_signal(c1,UNO_cancel_enter_room);
			return true;

	}
	return false;//����UNO�ź���Ϣ
}

#endif