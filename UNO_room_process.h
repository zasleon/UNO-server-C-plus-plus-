#ifndef INCLUDE_UNO_room_process_H
#define INCLUDE_UNO_room_process_H

#include"server_general_function.h"

void UNO_clean_player_card(UNO_player* p1)//������ֿ����
{
	
	for(int i=0;i<UNO_deck_card_limit;i++)
	{
		p1->card_color[i]=UNO_none;
		p1->card_effect[i]=UNO_none;
	}
	
}
void UNO_ini_room()//���з����ʼ��
{
	UNO_empty_room_pointer=0;//ָ���һ��
	int i,j;
	for(i=0;i<UNO_room_limit;i++)
	{
		uno_room[i].room_member=0;//����������Ϊ0
		uno_room[i].who_is_master=-1;//����Ϊ��
		for(j=0;j<UNO_member_limit;j++)
		{
			uno_room[i].player[j].room_member_NO=j;//�Լ��Ƿ����ڵڣ���0��ʼ�����û�
			uno_room[i].player[j].state=UNO_empty;//λ��״̬��Ϊ��
			uno_room[i].player[j].room_No=i;//����Ŷ�Ӧ
			uno_room[i].player[j].whether_get_card=false;//ȫ����Ϊû�����״̬
			uno_room[i].player[j].think_time=0;
			uno_room[i].player[j].not_declare_UNO_time=0;
			uno_room[i].player[j].whether_declare_UNO=false;
			uno_room[i].player[j].whether_choosing_color=false;
			UNO_clean_player_card(&uno_room[i].player[j]);

			uno_room[i].AI_thread[i]=NULL;//AI�߳����
			uno_room[i].game_start=false;//��Ϸû׼����ʼ
			uno_room[i].game_over=true;//��Ϸ�Ƿ������falseΪ��û����������Ϸ��ʼ����׼������ʱgame_overҲΪtrue����ʾ��Ϸû����
			uno_room[i].whether_lock=false;//û������������
			

			//AI��������
			uno_room[i].player[j].only_color=UNO_none;
			uno_room[i].player[j].not_have_color=UNO_none;
			uno_room[i].player[j].i_want_play_this_color=UNO_none;//Ĭ��Ϊɶ��ɫ�������
			
			
		}
		
	}
}

void UNO_tips(UNO_player* p1,int signal)
{
	//��ǰû�г鿨��ѡ�񡰹�����������ʾ�鿨UNO_please_get_card
	//��ǰ�мӷ���ѡ���˳鿨����ʾֱ��ѡ������UNO_got_punish_choose_pass
	//��ǰ���Լ��غϣ��Լ����Ʋ����ϣ�������ʾ
	//��ǰ�Ѿ������ƣ���ѡ�񡰹���
	if(p1->state==UNO_human)
		send_msg_signal(p1->c1,signal);
	Sleep(500);
}
void UNO_update_all_info_to_himself(UNO_room* this_room,UNO_player* p1);
char* UNO_get_client_message(UNO_player* p1,int time_length)//��ȡ�ͻ��˷��͵���Ϣ����ʱ������ʱ��time_length,,��ͨ����ʱ��ѡ����ɫʱ�õ�����ʹ��֮���ѡ����г�ʱ�ж�ǿ��Ĭ��ѡ��
{
	if(p1->state!=UNO_human)return "";//���ǻ��ˣ�c1Ϊ��ָ��
	client_member* c1=p1->c1;
	memset(c1->p_message,0,sizeof(c1->p_message));
	Sleep(10);
	//cout<<"��Ϸ����ȴ��û�����\n";
	int wait_time=0;
	
	while(strlen(c1->message)==0)
	{
		if(c1->F_offline)return "";
		Sleep(100);

		//������˼��������ѡ���ǰ�����Լ��غ��ˣ�˵���غϱ����ˡ�˵�����˴������ͬ�ĺ�ɫ��,��˴��Լ�ѡ����ɫ�������ˡ�
		//���������Ӧ�ò��ᷢ������Ϊ��������ƺ��������߳������������õ�һ��������Ƶ���ר��ѡ��ɫ��ֻ���ܳ�ʱ

		if(uno_room[p1->room_No].game_over) return "";//�����Ϸ�������˳�����
		
		if(p1->card_color[1]==UNO_none&&p1->whether_declare_UNO==false)//���û���޶�ʱ����û��uno���ӷ�2�ſ�
		{
			p1->not_declare_UNO_time+=100;
			if(p1->not_declare_UNO_time>=UNO_declare_time_limit)
			{
				//cout<<"û��uno���ӷ�2��!\n";
				return "";
			}
			
		}
		else
			p1->not_declare_UNO_time=0;

		if(uno_room[p1->room_No].whose_turn==p1->room_member_NO)//�����ǰ���Լ��غ�,��ʱ�ȴ�
		{
			p1->think_time+=100;
			if(p1->think_time==2000||p1->think_time%10000==0)//����ÿ��һ��ʱ��ˢ�£���ֹ�ͻ���û�յ���Ϣ
				UNO_update_all_info_to_himself(&uno_room[p1->c1->room_No],p1);
			if(p1->think_time>=time_length)//����ȴ���ʱ
			{
				//cout<<"�û����볬ʱ!\n";
				return "";
			}
		}
		else
			p1->think_time=0;
	}//���еȴ�

	//ִ�е������Ȼ�յ�����
	addtext(c1->p_message,c1->message);
	c1->not_be_read=false;
	Sleep(5);
	//cout<<"get msg:"<<c1->p_message<<endl;
	return c1->p_message;
}


void UNO_add_room_message(UNO_room* this_room,JSON_package* json_msg)//��ӷ��ڳ�Ա��Ϣ��json
{
	char StrBuf[MAX_PATH] = { 0 };
	json_msg->add_item("master_number",this_room->who_is_master);//˭�Ƿ���
	for(int count=0;count<UNO_member_limit;count++)//ͳ�Ʒ�������Ա����
	{
		switch(this_room->player[count].state)
		{
			case UNO_empty://addtext(Str_member,"���ա� ");
				json_msg->add_item_array("room_member","name","���ա�");
				break;
			case UNO_human:
				memset(StrBuf,0,sizeof(StrBuf));
				if(this_room->who_is_master==count)addtext(StrBuf,"��������");
				addtext(StrBuf,this_room->player[count].c1->client_name);
				json_msg->add_item_array("room_member","name",StrBuf);
				break;
			case UNO_simple_AI:
				//addtext(Str_member,"���򵥻����ˡ� ");
				json_msg->add_item_array("room_member","name","�򵥻�����");
				
				break;
			case UNO_hard_AI:
				memset(StrBuf,0,sizeof(StrBuf));addtext(StrBuf,"һ�������");addtext(StrBuf,this_room->player[count].room_member_NO+1);addtext(StrBuf,"��");
				json_msg->add_item_array("room_member","name",StrBuf);
				
				break;
			default:
				json_msg->add_item_array("room_member","name","��ֶ���");
				break;
		}
	}
}

void UNO_refresh_room_member_to_all(int signal,UNO_room* this_room,client_member* c1)//���·�������Ա���ݣ������¼���,�����¼�
{
	//UNO_add_full_robot_success
	//UNO_create_room_success
	//UNO_enter_room_success
	//UNO_someone_leave_room

	JSON_package json_msg,json_msg2;
	json_msg.add_item("event_name",c1->client_name);//�����¼�������
	json_msg2.add_item("event_name",c1->client_name);//�����¼�������

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
	
	
	UNO_add_room_message(this_room,&json_msg);//��ӷ������г�Ա��Ϣ
	UNO_add_room_message(this_room,&json_msg2);

	client_member* c2;
	for(int i=0;i<UNO_member_limit;i++)
		if(this_room->player[i].state==UNO_human)//����ǻ��ˣ�������Ϣ����
		{
			c2=this_room->player[i].c1;
			if(c2->member_No==c1->member_No)
			{
				json_msg.add_item("your_number",i);
				send_msg(c2,json_msg.to_StrBuf());//���������г�Ա��Ϣ
			}
			else
			{
				json_msg2.add_item("your_number",i);
				send_msg(c2,json_msg2.to_StrBuf());//���������г�Ա��Ϣ
			}

			
		}
}

void UNO_add_FULL_robot_to_room(client_member* c1)//����Ƿ�����������ӻ�����
{

	int who_is_master=uno_room[c1->room_No].who_is_master;//��ȡ�������
	client_member* master;
	master=uno_room[c1->room_No].player[who_is_master].c1;//ȷ�Ϸ�����ɫ
	if(who_is_master==-1){cout<<"����û�У�\n";return;}
	if(master!=NULL)//
	{
		if(uno_room[c1->room_No].player[who_is_master].c1->member_No!=c1->member_No)//������Ƿ������ˣ���Ȩ�޲���������
		{
			send_msg_signal(c1,UNO_not_got_room_operate_right);
			return;
		}
	}
	else
	{cout<<"����Ϊ�գ�"<<who_is_master<<endl;return;}
	
	uno_room[c1->room_No].room_member=UNO_member_limit;//ֱ������Ϊ��Ա
	for(int i=0;i<UNO_member_limit;i++)
		if(uno_room[c1->room_No].player[i].state==UNO_empty)
			uno_room[c1->room_No].player[i].state=UNO_hard_AI;//��ɫ���Ϊһ�������
	UNO_refresh_room_member_to_all(UNO_add_full_robot_success,&uno_room[c1->room_No],c1);
	

}

void UNO_create_room(client_member* c1)//�û���������
{
	if(UNO_room_in_use==UNO_room_limit)//�������ȫ��
	{send_msg_signal(c1,UNO_room_full);return;}

	int this_room_number=UNO_empty_room_pointer;
	
	while(uno_room[this_room_number].who_is_master!=-1)
	{
		cout<<"UNO ���󣡣���\n";
		Sleep(100);
		this_room_number=UNO_empty_room_pointer;
		if(UNO_room_in_use==UNO_room_limit)//����ʧ��!
		{send_msg_signal(c1,UNO_room_full);return;}
	}

	//����һ����Ȼ�ܴ�������
	uno_room[this_room_number].who_is_master=0;//��ǰ���䷿������Ϊλ��0��
	UNO_room_in_use++;

	int temp=UNO_empty_room_pointer;
	if(UNO_room_in_use!=UNO_room_limit)//�������û��
		while(true)//��Ѱ��һ���շ��䣬����UNO_empty_room_pointer
		{
			temp++;//����һ������
			if(temp==UNO_room_limit)temp=0;//���������һ�����䣬��ͷ��ʼ��

			if(uno_room[temp].who_is_master==-1)//����÷���Ϊ��
			{
				UNO_empty_room_pointer=temp;//ȷ�Ϸ���ţ�����UNO_empty_room_pointer
				break;//����ѭ��
			}
		
		}

	uno_room[this_room_number].player[0].c1=c1;
	uno_room[this_room_number].player[0].state=UNO_human;//��λ��״̬����Ϊ�������

	c1->room_No=this_room_number;//ȷ��������ڷ����
	c1->guest=false;
	UNO_refresh_room_member_to_all(UNO_create_room_success,&uno_room[this_room_number],c1);
	c1->state=UNO_in_room;//�������״̬Ϊ����uno�����ڡ�
	uno_room[this_room_number].room_member++;//����������Ϊ1
	
}


void UNO_show_room_in_use(client_member* c1)//����չʾ�����ܼ���ķ���
{
	if(UNO_room_in_use==0)//���û��ʹ���ŵķ���
	{send_msg_signal(c1,UNO_no_room_in_use);return;}

	JSON_package json_msg;
	json_msg.add_item("signal",UNO_show_roomlist);
	JSON_package room_msg;

	for(int count=0;count<UNO_room_limit;count++)
		if(uno_room[count].who_is_master!=-1)
		{
			room_msg.add_item("room_No",count+1);//�����
			room_msg.add_item("master",uno_room[count].player[uno_room[count].who_is_master].c1->client_name);//��������+����
			room_msg.add_item("room_member",uno_room[count].room_member);//��������

			json_msg.add_struct_array("room_msg",room_msg.get_json());
		}
	send_msg(c1,json_msg.to_StrBuf());//����
}

void UNO_enter_room(client_member* c1,JSON_package json_msg)//�û�������˵ķ���
{
	if(UNO_room_in_use==0)//���û��ʹ���ŵķ���
	{send_msg_signal(c1,UNO_no_room_in_use);return;}
	

	//ִ�е������Ȼ��ѡ����˵ķ��䲢����

	int choice=atoi(json_msg.get_value("which_room"));//���������ѡ��ת������Ϊ���� 
						
		if(choice==0)//confirm_send_success(c1,"��������Чֵ!\n");
		{return;}
		if(choice>UNO_room_limit||choice<0)//confirm_send_success(c1,"���볬��������������!\n");
		{return;}

		choice--;
		
		if(uno_room[choice].who_is_master==-1)//confirm_send_success(c1,"�÷�����û��!\n");
		{send_msg_signal(c1,UNO_this_room_is_empty);return;}
		if(uno_room[choice].room_member==UNO_member_limit)//confirm_send_success(c1,"�÷���������!\n");
		{send_msg_signal(c1,UNO_this_room_is_full);return;}
		if(uno_room[choice].game_start==false&&uno_room[choice].game_over==false)//������������Ϸ����û����
		{send_msg_signal(c1,UNO_room_is_in_game);return;}
		
		//ִ�е���һ����Ȼ����ɹ�
		uno_room[choice].room_member++;//����������+1
		c1->guest=true;//���뷿������Ƿ������
		c1->room_No=choice;
		c1->state=UNO_in_room;//�������״̬Ϊ����uno�����ڡ�

		for(int i=0;i<UNO_member_limit;i++)
			if(uno_room[choice].player[i].state==UNO_empty)//�����ǰλ��Ϊ��
			{
				uno_room[choice].player[i].state=UNO_human;//��λ��״̬����Ϊ�������
				
				uno_room[choice].player[i].c1=c1;//��ʶ
				GetLocalTime(&uno_room[choice].player[i].enter_time);//��ȡ��ǰʱ�䣬���ڷ����䶯
				break;
			}

		c1->room_No=choice;//���淿�����Ϣ
		UNO_refresh_room_member_to_all(UNO_enter_room_success,&uno_room[choice],c1);//ˢ�³�Ա
		c1->state=UNO_in_room;

}

bool compare_enter_time(SYSTEMTIME challenger,SYSTEMTIME former_master)//����true��ʾǰ�߱Ⱥ��ߴ�
{
	bool result=false;//���ȷ�������
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





void UNO_exit_room(client_member* c1)//�û��˳�����
{
	
	char StrBuf[MAX_PATH] = { 0 };

	int who_will_be_the_master=-1;//����������������ᴥ��


	UNO_room* this_room=&uno_room[c1->room_No];
	
	for(int i=0;i<UNO_member_limit;i++)
		if(this_room->player[i].c1!=NULL&&this_room->player[i].c1->member_No==c1->member_No)//�ҵ�Ҫ�˳���
		{
			
			if(this_room->who_is_master==i)//����˳���Ϊ��������,����������ʩ
			{
				for(int j=UNO_member_limit-1;j>=0;j--)
					if(i!=j&&this_room->player[j].state==UNO_human)//������λ�Ӽ�飬����ǰλ�����ǻ���
					{
						if(who_will_be_the_master==-1)//�����ǰ��ûȷ�Ϲ������򲹣�����ǰλ���ϵ�����Ϊ������
							who_will_be_the_master=j;
						else//����з������ˣ��Ƚ����ߵĽ��뷿��ʱ��
						{
							if(compare_enter_time(this_room->player[j].enter_time,this_room->player[who_will_be_the_master].enter_time))
								who_will_be_the_master=j;
						}

					}
				this_room->who_is_master=who_will_be_the_master;//ȷ���·���
				if(who_will_be_the_master!=-1)//����ǰ���䲻Ϊ��
					this_room->player[who_will_be_the_master].c1->guest=false;
				else//û����,�÷���û��ʹ��
				{
					if(this_room->game_start)//���˵��Ϸ���ڽ�����
					{
						this_room->game_over=true;
						Sleep(600);//�ȴ�AI�߳̽���
						this_room->game_start=false;
					}
					for(int k=0;k<UNO_member_limit;k++)
					{
						this_room->player[k].state=UNO_empty;//λ��״̬��Ϊ��,���������
					}
					this_room->room_member=0;//ֱ����0����Ϊ������Ҳ������
					UNO_room_in_use--;//�ܷ���ʹ������-1
				}
			}//���������������
			
			this_room->player[i].c1=NULL;//����ȡ������
			
			if(this_room->who_is_master!=-1)//��������ڻ�����
				this_room->room_member--;//����������-1
			else//������û����
			{
				//һ��û����˵���Ƿ����뿪���䣬������û�˽����ڷ������������ִ��
			}
			c1->guest=false;
			this_room->player[i].state=UNO_empty;//��ǰλ��״̬����Ϊ��

			send_msg_signal(c1,UNO_leave_room_success);//�뿪�ɹ�!
			c1->state=in_online;//�����û�״̬Ϊ�����ߡ�

			//�ó�Ա�뷿�������������֪ͨ�����������˸ý�ɫ��ȥ
			if(uno_room[c1->room_No].room_member!=0)//��������л�����
			{
				UNO_refresh_room_member_to_all(UNO_leave_room_success,this_room,c1);//�������·�������Ա����
			}
			
			
			break;
		}


}




#endif