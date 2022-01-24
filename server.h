#ifndef INCLUDE_server_H
#define INCLUDE_server_H

#include "SZSC_service.h"
#include "movie_function.h"
#include "UNO_service.h"


//#pragma comment(lib,"F:\\VC6.0PROJECT\\szsc_server\\Debug\\fight_room.lib")
//_declspec(dllexport) void start_fight(client_member* p,fight_room* this_room);

//����ͨѶ
void communicate_with_this_client_O(client_member* c1)//��������"ϵͳ������\n"
{
	//send(g_ClientSocket[c1->member_No-1],"ϵͳ������\n", (1 + ::strlen("ϵͳ������\n")), 0);
	send_msg_signal(c1,system_overload);
}

void communicate_with_this_client(client_member* c1)//��������ͻ��˽��й�ͨ
{
	char StrBuf[MAX_PATH] = { 0 };//��������洢���յ�������
	char Buf[MAX_PATH] = { 0 };//ר������ת�����ַ���ʽ��ֵΪ�ַ�
	

	c1->listen_this=CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)listen_client_message,c1, NULL, NULL);//��ʼ�����߳�

	send_msg_signal(c1,start_link);//��ʾ�Լ������ṩ����

	memset(StrBuf,0,sizeof(StrBuf));
	addtext(StrBuf,get_client_message(c1));if(c1->F_offline){cout<<"������\n";client_offline(c1);return;}//��ȡ�û�������Ϣ���������������ͨ
	
	JSON_package start_msg(StrBuf);
	switch(atoi(start_msg.get_value("device_type")))
	{
		case android_phone:c1->device=android_phone;cout<<"��׿���豸!\n";break;
		case win_console:c1->device=win_console;cout<<"win_console���豸!\n";break;
		default:cout<<"δ��ʶ����豸!ǿ�ƶϿ�����!\n";client_offline(c1);return;
	}


	//memset(StrBuf,0,sizeof(StrBuf));addtext(StrBuf,"���ǵ�");addtext(StrBuf,client_number);addtext(StrBuf,"��������\n");confirm_send_success(c1,StrBuf);

	memset(c1->client_name,0,sizeof(c1->client_name));
	memset(StrBuf,0,sizeof(StrBuf));
	addtext(StrBuf,start_msg.get_value("username"));
	cout<<"�û���:"<<StrBuf<<"�̺߳�"<<c1->member_No<<endl;
	if(strlen(StrBuf)<=0)
	{
		send_msg_signal(c1,username_too_short);//�û�������
		client_offline(c1);return;
	}
	if(strlen(StrBuf)>sizeof(c1->client_name))//sizeof()���������������С��strlen()��ͳ�Ƶ�ǰ�ַ������ȵ�\0Ϊֹ
	{
		send_msg_signal(c1,username_too_long);//�û����볤�ȳ�������!
		client_offline(c1);return;
	}

	//ִ�е������Ȼ��˳���Ϸ���¼
	send_msg_signal(c1,login_success);
	addtext(c1->client_name,StrBuf); //ȷ���û���
	for(int i=0;i<sizeof(c1->client_name);i++)
	{
		if(c1->client_name[i]=='\"')
		{
			c1->client_name[i]=' ';
			for(int j=i;j<sizeof(c1->client_name)-1;j++)
				c1->client_name[j]=c1->client_name[j+1];
		}
		if(c1->client_name[i]=='\n')
			c1->client_name[i]='\0';
	}

	while(true)//ѡ��ʼ��ѭ��
	{
			
			//"\n�������������ѡ��\n";
			//"|-------------------------------------------------------------------------|\n";
			//"|  ѡ1�������Լ��ķ���         ѡ2��������˵ķ���           ѡ3���˳�����|\n";
			//"|                                                                         |\n";
			//"|  ѡ3���˳�����               ѡ4��ˢ������б�           ѡ5����ʼս��|\n";
			//"|-------------------------------------------------------------------------|\n";
			//"��ѡ��:";
		//if(c1->device!=android_phone)special_signal(c1,show_choice);

		//cout<<"�����濪ʼ�����û�"<<c1->client_name;
		memset(StrBuf,0,sizeof(StrBuf));
		addtext(StrBuf,get_client_message(c1));
		Sleep(100);
		if(c1->F_offline){client_offline(c1);return;}//��ȡ�û�������Ϣ���������������ͨ
			
		switch(c1->state)
		{
			case in_game://���ж�ͨ������Ȼ�Ƿ�����ͽ�����SZSC��Ϸ
				start_fight(c1,&player_room[c1->room_No]);
				c1->state=in_room;//��Ϸ�������ص�����״̬
				//if(c1->device!=android_phone)special_signal(c1,show_choice);
				continue;
			case UNO_in_game://���ж�ͨ������Ȼ�Ƿ������guest������UNO��Ϸ
				UNO_start_one_game(c1,false);
				if(!c1->F_offline)
				c1->state=UNO_in_room;//��Ϸ�������ص�����״̬
			continue;
			
		}


		JSON_package json_msg(StrBuf);
		//cout<<"�������յ���������"<<StrBuf<<endl;
		
		if(UNO_provide_service(c1,json_msg))//�����UNO��Ϸ�����������·���
			continue;
		if(SZSC_provide_service(c1,atoi(StrBuf)))//�����SZSC��Ϸ�����������·���
			continue;

		switch(atoi(StrBuf))
		{
			
			case 8:
				//movie_F(c1);
				break;

			case apply_refresh_online_member://ˢ������б�
				if(c1->device==win_console)special_signal(c1,please_cls);
				show_all_client(c1);//ˢ���б�
				break;
			case 0:

				send_msg_signal(c1,you_are_in_the_lobby);//���ش������Ƿ���0���������Ѿ��ڴ�����
				break;
			default:
				if(c1->device==win_console)confirm_send_success(c1,"��������Чֵ!\n");
				
		}//switch����
					
	}//ѡ��ʼ��ѭ������
}

void accept_client()//���м��������ܿͻ��˷�����Ϣ������
{
	char StrBuf[MAX_PATH] = { 0 };//��������洢���յ�������
	char Buf[MAX_PATH] = { 0 };//ר������ת�����ַ���ʽ��ֵΪ�ַ�
	
	/*���Ĳ� ���ܿͻ���������*/
	sockaddr_in ClientAddr[memberlimit+1]= { 0 };//�����ṹ�� ���ڻ�ȡ�ͻ��� socketaddr �ṹ��
	int len=sizeof(ClientAddr);//��ȡ�ṹ���С
	bool claim_limit=false;//�Ƿ���ʾ�����������������ﵽ����

	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)server_monitor,NULL, NULL, NULL);//����ԱȨ�޲鿴����
	//Sleep(500);
	while(true)//��ʼ������ѭ��
	{	
		check_emptyspace();

		if(empty_player_pointer>=memberlimit)//�������������Ƿ����أ��������
		{
			if(!claim_limit)
			{
				cout<<"���������������ﵽ���!�޷��ټ�������!\n";claim_limit=true;//��Ϊ����ʾ���������
			}
			//���Ͻ��ܹ������������������ظ����������½�ʱ�������ĵ�һ���ͻ��˽��ᱻ����
			g_ClientSocket[memberlimit]= accept(g_ServerSocket, (sockaddr*)&ClientAddr[memberlimit], &len);
			hthread[memberlimit]=CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)communicate_with_this_client_O,&Membership[memberlimit], NULL, NULL);//���ú�����ÿͻ��˽��й�ͨ
		}
		else//������û���أ����м��������ܿͻ��˷�����Ϣ������
		{
			if(claim_limit)//���֮ǰ�����������������ˣ����ڼ�鵽��λ����ʾ���������Լ������ܿͻ���������
			{
				CloseHandle(hthread[memberlimit]);
				claim_limit=false;
				cout<<"���������������½�!���Լ������տͻ�����!\n";
			}

			cout<<"���������С�����Ŀǰ��"<<client_number<<"���ͻ��������ӣ�����Ϊ"<<memberlimit<<"����\n";
			g_ClientSocket[empty_player_pointer]= accept(g_ServerSocket, (sockaddr*)&ClientAddr[empty_player_pointer], &len);//���ܿͻ��˵���������
			
			SOCKADDR_IN   sockAddr; 
			int   iLen=sizeof(sockAddr); 
			getpeername(g_ClientSocket[empty_player_pointer] ,(struct   sockaddr   *)&sockAddr,&iLen);//��ȡ�ͻ��˵�ip
			cout<<"\n���û������ӣ��ͻ���IP: "<<::inet_ntoa(sockAddr.sin_addr)<<"�˿ں�: "<<ntohs(sockAddr.sin_port)<<endl;


			Membership[empty_player_pointer].member_in_use=true;//����������λ�ñ�Ϊ����ռ�á�
			client_number++;
			cout<<"�ѽ��ܿͻ����������󡭡�\n";
			
		/*���岽 ��������*/
			//�̣߳���ȫ���ԣ���ʼջ��С�����������������Σ��߳�״̬��־���߳�id�����ַ��
			//�߳�״̬��־��CREATE_SUSPENDED����/0��������
			if(hthread[empty_player_pointer]!=NULL)
			{
				CloseHandle(hthread[empty_player_pointer]);//��ֹ���������
			}
			hthread[empty_player_pointer]=CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)communicate_with_this_client,&Membership[empty_player_pointer], NULL, NULL);//���ú�����ÿͻ��˽��й�ͨ

		}//�������¿ͻ��˵Ĺ�ͨ���ӽ��������¼��������Ƿ����أ�������ֹͣ������û��������������Ƿ�����һ���ͻ��˵�����

	}//������ѭ�����������Ƿ������رգ�������ѭ����һֱ������ȥ��

}







#endif