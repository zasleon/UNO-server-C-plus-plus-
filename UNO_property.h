
#ifndef INCLUDE_UNO_property_H
#define INCLUDE_UNO_property_H

//��ҵ�ǰ״̬
//#define	in_online					6800//��������״̬
#define UNO_in_room						6811//����uno������
#define UNO_in_game						6812//����uno��Ϸ��
//#define UNO_in_roomlist				6813//����ѡ��uno�����б���

//������ĳһλ�ӵ�״̬
#define	UNO_empty		0//��λ��Ϊ��
#define UNO_human		1//�������
#define UNO_simple_AI	2//�򵥻�����
#define UNO_hard_AI		3//���ѻ�����

//��Ϸ��������
#define UNO_member_limit			4//һ����Ϸ�����������
#define UNO_room_limit				100//������������
#define UNO_deck_card_limit			108//�����ж����ſ�
#define UNO_user_choose_time_limit	1000*930//�û����Լ��غϵ�˼��ʱ������

//��Ϸʱ������
#define	UNO_declare_time_limit	1000*4//�ȴ�����uno��ʱ�䣬��ʱ��ӷ�2�ſ�
#define UNO_AI_think_time		1000*3//AIִ�ж���ǰ�ȴ����
#define UNO_AI_wait_time		500//AI�ȶ�ò鿴һ���Ƿ��ǵ�ǰ�Լ��غ�

//��ɫ
#define UNO_green	1
#define UNO_red		2
#define UNO_blue	3
#define UNO_yellow	4
#define UNO_black	5

//���⹦��
#define UNO_none		-1//����Ϊ��
#define UNO_ADD_2		100//�ӷ�2��Ƭ
#define UNO_ADD_4		200//�ӷ�4��Ƭ
#define UNO_reverse		300//����
#define UNO_forbid		400//������һ����
#define UNO_universal	500//����

//��ǰЧ��
#define UNO_add_card	100//�ӷ���Ƭ
//#define UNO_forbid		400//������һ����

#define UNO_clockwise		1
#define UNO_anti_clockwise	2


struct client_member;
struct UNO_player
{
	client_member* c1;
	char	name[30];
	int		state;//״̬��������������
	int		card_effect[UNO_deck_card_limit];//�ֿ�����
	int		card_color[UNO_deck_card_limit];

	bool	whether_declare_UNO;//�Ƿ����ƹ�UNO��,��ʼ���������ƺ�����Ϊfalse
	//bool	card_can_be_played[UNO_deck_card_limit];//ȷ�Ͽɴ���Ŀ���trueΪ��ǰ�ɴ����falseΪ��ǰ�޷����

	SYSTEMTIME	enter_time;//���뷿��ʱ��
	int		room_No;//��Ҷ�Ӧ�����
	int		room_member_NO;//�Լ��Ƿ����ڵڼ����û�

	bool	whether_choosing_color;//���˺��ƣ��Լ���ǰ�Ƿ�����ѡȡ��ɫ
	bool	whether_get_card;//��ǰ�Ƿ��������ˣ�
	int		think_time;//�û�������ѡ���ʱ��
	int		not_declare_UNO_time;//�û�û����UNO��ʱ��

	int		only_color;//�������ڳ�������ѡ����ɫ���Լ�ֻʣһ���ƣ���¼��ѡ������ɫ
	int		not_have_color;//��һ�����ѡ�����ƣ�������û�������ϵ�ǰ��ɫ���ƣ����м�¼
	int		i_want_play_this_color;//AI˼��������Լ�������ɫ���ƱȽ϶࣬���ȼ�¼������ɫ����������Լ���ʹ�ú��ƣ���ѡ�Ǹ���ɫ
	
};

static int UNO_room_in_use;//��ǰ��ʹ�õķ�������
static int UNO_empty_room_pointer;
struct UNO_room
{
	int		deck_card_effect[UNO_deck_card_limit];//����
	int		deck_card_color[UNO_deck_card_limit];

	int		tomb_card_effect[UNO_deck_card_limit];//�Ͽ���
	int		tomb_card_color[UNO_deck_card_limit];

	UNO_player player[UNO_member_limit];
	int		who_is_master;//-1-4,˭�Ƿ�����-1��ʾ����Ϊ��
	int		room_member;//��ǰ�ڷ�������Ա��������

	int		whose_turn;//��ǰΪ˭�Ļغ�
	int		last_color;//���һ�ų����Ƶ���ɫ
	int		last_effect;//���һ�ų����Ƶ�Ч��
	int		current_running_effect;//��ǰ�Ƿ���¼�ʩ��Ч����ʩ�Ӻ����ΪUNO_none
	int		punish_card_number;//�ӷ��Ŀ�Ƭ����
	int		current_color;//��ǰ�ɷ�������ɫ
	bool	clock_direct;//true��˳ʱ����ƣ�false����ʱ��

	HANDLE	AI_thread[UNO_member_limit];//AI˼���̣߳�����Ϸ�����򷿼�ճ�ʱ�����
	bool	whether_lock;//�Ƿ��Ѿ������������߳�

	bool	game_start;//��ʶ����һ�о�����Ϸ��ʼ��Ϊtrue������Ϊfalse���ڷ���ִ������Ϸ��ʼ���������ݺ�game_start��Ϊtrue��֪ͨ������׼�����������������ˣ�
	//����Ϸ����ʱ�򷿼��ʣ�����һ����������Ϸ�ж����뿪ʱ����ĸñ�־��,����Ϸ��ʼ��׼���������ʱ����
	bool	game_over;//��ʶ���������Ϸ�����������˽���˼���߳�
	//����Ϸ����ʱ�򷿼��ʣ�����һ����������Ϸ�ж����뿪ʱ����ĸñ�־��,����Ϸ��ʼ��׼���������ʱ����
	
};

UNO_room uno_room[UNO_room_limit];
//�û��ᷢ�����ź�
#define UNO_apply_create_room			2000
#define	UNO_apply_enter_room			2001
#define UNO_apply_exit_room				2002
#define UNO_apply_add_robot				2003
#define UNO_apply_start_game			2004
#define UNO_apply_add_full_hard_robot	2005//���������һ�������
#define UNO_apply_enter_this_room		2006
#define UNO_apply_exit_roomlist			2007

//ϵͳ��ظ��û����ź�
#define UNO_room_full					2050//����������û��������������
#define UNO_no_room_in_use				2051//��ǰû�з�����ʹ�ã��޷����뷿��
#define UNO_create_room_success			2052//��������ɹ�
#define UNO_enter_room_success			2053//���뷿��ɹ�
#define UNO_someone_get_in				2054//���˽�������
#define UNO_show_roomlist				2055//��ͻ���չʾ�����б�
#define UNO_leave_room_success			2056//�뿪����ɹ�
#define UNO_show_roomlist_end			2057//��ʾ�������
#define	UNO_cannot_exit_room			2058//���ڷ����ڣ���ǰ״̬�޷��˳�uno����
#define	UNO_cannot_enter_room			2059//��ǰ״̬�޷�������˵�uno����!
#define	UNO_cannot_create_room			2060//��ǰ״̬�޷�����uno����!
#define UNO_cancel_enter_room			2061//ȡ�����������˵ķ���
#define UNO_someone_leave_room			2062//�����뿪������
#define UNO_you_become_host				2063//���Ϊ��������
#define UNO_refresh_room_member			2064//ˢ�·�������Ա�б�
#define UNO_this_room_is_full			2065//��ǰ��������
#define UNO_this_room_is_empty			2066//��ǰ����Ϊ��
#define UNO_add_full_robot_success		2067//������ȫ��������
#define UNO_not_got_room_operate_right	2068//ûȨ�޶Է����Ա���в���
#define UNO_lack_of_players				2069//���俪����Ϸ��������
#define UNO_game_start					2070//���߷���������Ա��Ϸ��ʼ��
#define UNO_room_is_in_game				2071//������������Ϸ���޷�����
#define UNO_already_in_room				2072//���Ѿ��ڷ�������
#define UNO_cannot_operate_this			2073//���ܽ��д˴β���


//��Ϸ���ź�
#define UNO_apply_get_card				2020//�����һ�ſ�
#define UNO_i_only_got_one_card			2021//��ֻʣһ������
#define	UNO_apply_pass					2022//��������һ���˳���
#define UNO_apply_play_this_card		2023//���������


//������δʵ�֣����ͻ����յ������ź�ʱ��չʾ����Ч��
#define UNO_direct_change				2085//����ı䣬˳ʱ�����ʱ��
#define	UNO_someone_be_forbidden		2086//���˱�������
//һ�㹦����Ϣ
#define UNO_refresh_all_data			2095//����������Ϣ
#define	UNO_player_choose_color_change	2096//��ǰ���Գ�����ɫ����
#define UNO_please_choose_color			2097//�����ѡ����ɫ����ѡһ
#define UNO_add_game_log				2098//�����־��Ϣ

//��ʾ
#define UNO_please_get_card				2100//���û�ѡ�񡰹���ʱ��û�鿨ʱ���ѳ鿨
#define	UNO_got_punish_choose_pass		2101//��ǰ�мӷ�Ч������ֱ��ѡ�񡰹���
#define	UNO_cannot_play_this_card		2102//�Լ��غϵ�ǰ�޷�ʹ�ô˿�
#define UNO_got_card_please_choose_pass	2103//�Ѿ����������ѡ�񡰹���
#define	UNO_got_punish					2104//��ǰ�мӷ�Ч��!�޷�����˿�

//��Ϸ����
#define UNO_game_over_lack_player		2110//��Ϸ��Ϊ��Ա���������
#define UNO_game_over_someone_win		2111//��Ϸ��Ϊ��ʤ������






#endif