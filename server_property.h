#ifndef INCLUDE_server_property_H
#define INCLUDE_server_property_H




#include <time.h>
//#include <fstream>
#include "winsock2.h"
//#include <cstring>
//#include <iostream>
//#include <windows.h>

#include "UNO_property.h"//UNO��Ϸ����
#include "card_effect.h"//SZSC��Ϸ����
using namespace std;
#pragma comment(lib,"Ws2_32.lib")
//#include <WS2tcpip.h>




#define memberlimit					5000//������������������
#define sleeptime					50//��������΢�ӳٷ�������ʱ��


#define system_overload				9999//ϵͳ����
#define start_link					6666//���߿ͻ��˳ɹ�ͨѶ�����ˣ����ͻ����ղ����������ʱ������Լ������������ܳ����ˡ���
#define please_cls					6667//�ÿͻ���ˢ����Ļsystem("cls");
#define show_choice					6668//�ÿͻ�����ʾѡ�����
#define show_user					6669//�ÿͻ�����ʾ�����û�״̬
#define	stop_show_user				6700//ֹͣ��ʾ�����û�״̬
#define client_get_message			6701//�ͻ��˷����Լ����յ���Ϣ
#define show_video					6702//��Ƶ����

#define	username_too_short			6725
#define username_too_long			6726
#define login_success				6727//�ܺϷ�˳����¼��

#define	in_online					6800//��������״̬
#define	in_game						6801//������Ϸ״̬
#define in_room						6802//�����ڷ���״̬
#define in_roomlist					6803//�����ڷ���״̬

//#define UNO_in_room				6811//����uno������
//#define UNO_in_game				6812//����uno��Ϸ��
//#define UNO_in_roomlist			6813

#define force_offline				7000//�ͻ���������
#define you_are_in_the_lobby		7000//�û�����0ʱ������ص��û�����

#define apply_refresh_online_member	-4//����ˢ��ҳ��



#define android_phone				9999//��׿�ֻ���
#define win_console					9998//win32����̨��

int client_number;//��������Ա





struct UNO_player;
struct client_member//�ͻ���
{
	int member_No;
	bool member_in_use;//�ͻ��˸�λ���Ƿ�ռ�ã�falseΪ�գ�trueΪռ��
	char client_name[30];//�û���

	int device;//�ֻ����ߵ���
	int state;//�ͻ�״̬������/����/ս����
	int room_No;//�Լ����ڷ����
	bool guest;//�ڷ������Ƿ��Ǳ��ͣ�trueΪ���ͣ�false����Ϊ���˻��ڷ�����

	bool F_offline;//�Ƿ����

	player* p1;
	player body;

	

	bool be_locked; //�Ƿ����������ÿͻ��˷��������ݣ�

	char message[MAX_PATH];
	bool not_be_read;//�ͻ��˷�����message�Ƿ�û��ϵͳ������������trueΪû������falseΪ����
	bool listen;//�Ƿ������
	HANDLE listen_this;//ָ�򡰼����ͻ��˷������ݡ����̣߳���accept�ͻ��˺󴴽����̲߳��������ָ��
	char p_message[MAX_PATH];//ϵͳ���ͻ��˷�������Ϣ�ȴ洢��message��Ҫ����ʱ����p_message����

};

client_member Robot[roomlimit];//��������ˣ�SZSCר��


client_member Membership[memberlimit+1];
SOCKET g_ServerSocket, g_ClientSocket[memberlimit+1]; //��������� �ͻ����׽���
HANDLE hthread[memberlimit+1];
















void addtext(char* StrBuf,int number);
void addtext(char* StrBuf,char* Buf);
void addtext(char* StrBuf,float number);


#endif