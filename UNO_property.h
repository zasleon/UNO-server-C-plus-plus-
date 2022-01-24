
#ifndef INCLUDE_UNO_property_H
#define INCLUDE_UNO_property_H

//玩家当前状态
//#define	in_online					6800//处于在线状态
#define UNO_in_room						6811//处于uno房间中
#define UNO_in_game						6812//处于uno游戏中
//#define UNO_in_roomlist				6813//处于选择uno房间列表中

//房间内某一位子的状态
#define	UNO_empty		0//该位子为空
#define UNO_human		1//人类玩家
#define UNO_simple_AI	2//简单机器人
#define UNO_hard_AI		3//困难机器人

//游戏常规设置
#define UNO_member_limit			4//一局游戏上限玩家数量
#define UNO_room_limit				100//房间上限数量
#define UNO_deck_card_limit			108//卡组有多少张卡
#define UNO_user_choose_time_limit	1000*930//用户在自己回合的思考时间限制

//游戏时间设置
#define	UNO_declare_time_limit	1000*4//等待他喊uno的时间，超时则加罚2张卡
#define UNO_AI_think_time		1000*3//AI执行动作前等待多久
#define UNO_AI_wait_time		500//AI等多久查看一次是否是当前自己回合

//牌色
#define UNO_green	1
#define UNO_red		2
#define UNO_blue	3
#define UNO_yellow	4
#define UNO_black	5

//特殊功能
#define UNO_none		-1//卡槽为空
#define UNO_ADD_2		100//加罚2卡片
#define UNO_ADD_4		200//加罚4卡片
#define UNO_reverse		300//反向
#define UNO_forbid		400//跳过下一个人
#define UNO_universal	500//万能

//当前效果
#define UNO_add_card	100//加罚卡片
//#define UNO_forbid		400//跳过下一个人

#define UNO_clockwise		1
#define UNO_anti_clockwise	2


struct client_member;
struct UNO_player
{
	client_member* c1;
	char	name[30];
	int		state;//状态栏，人类或机器人
	int		card_effect[UNO_deck_card_limit];//手卡内容
	int		card_color[UNO_deck_card_limit];

	bool	whether_declare_UNO;//是否宣称过UNO了,初始化和在摸牌后重置为false
	//bool	card_can_be_played[UNO_deck_card_limit];//确认可打出的卡，true为当前可打出，false为当前无法打出

	SYSTEMTIME	enter_time;//进入房间时间
	int		room_No;//玩家对应房间号
	int		room_member_NO;//自己是房间内第几个用户

	bool	whether_choosing_color;//出了黑牌，自己当前是否正在选取颜色
	bool	whether_get_card;//当前是否摸过牌了？
	int		think_time;//用户已作出选择的时间
	int		not_declare_UNO_time;//用户没声明UNO的时间

	int		only_color;//如果玩家在出万能牌选择颜色后自己只剩一张牌，记录他选出的颜色
	int		not_have_color;//若一个玩家选择摸牌，则该玩家没有牌桌上当前颜色的牌，进行记录
	int		i_want_play_this_color;//AI思考，如果自己哪种颜色手牌比较多，就先记录该种颜色，如果遇到自己想使用黑牌，就选那个颜色
	
};

static int UNO_room_in_use;//当前有使用的房间数量
static int UNO_empty_room_pointer;
struct UNO_room
{
	int		deck_card_effect[UNO_deck_card_limit];//卡组
	int		deck_card_color[UNO_deck_card_limit];

	int		tomb_card_effect[UNO_deck_card_limit];//废卡区
	int		tomb_card_color[UNO_deck_card_limit];

	UNO_player player[UNO_member_limit];
	int		who_is_master;//-1-4,谁是房主，-1表示房间为空
	int		room_member;//当前在房间内人员的总数量

	int		whose_turn;//当前为谁的回合
	int		last_color;//最后一张出的牌的颜色
	int		last_effect;//最后一张出的牌的效果
	int		current_running_effect;//当前是否对下家施加效果，施加后清空为UNO_none
	int		punish_card_number;//加罚的卡片数量
	int		current_color;//当前可发动的颜色
	bool	clock_direct;//true是顺时针出牌，false是逆时针

	HANDLE	AI_thread[UNO_member_limit];//AI思考线程，在游戏结束或房间空出时需结束
	bool	whether_lock;//是否已经锁死其他人线程

	bool	game_start;//标识符，一切就绪游戏开始则为true，否则为false。在房主执行完游戏初始化所有内容后game_start变为true，通知所有人准备就绪（包括机器人）
	//在游戏结束时或房间仅剩的最后一个活人在游戏中断线离开时会更改该标志符,在游戏开始的准备手续完成时更改
	bool	game_over;//标识符，如果游戏结束，机器人结束思考线程
	//在游戏结束时或房间仅剩的最后一个活人在游戏中断线离开时会更改该标志符,在游戏开始的准备手续完成时更改
	
};

UNO_room uno_room[UNO_room_limit];
//用户会发来的信号
#define UNO_apply_create_room			2000
#define	UNO_apply_enter_room			2001
#define UNO_apply_exit_room				2002
#define UNO_apply_add_robot				2003
#define UNO_apply_start_game			2004
#define UNO_apply_add_full_hard_robot	2005//房间里加满一般机器人
#define UNO_apply_enter_this_room		2006
#define UNO_apply_exit_roomlist			2007

//系统会回复用户的信号
#define UNO_room_full					2050//房间已满，没法继续创建房间
#define UNO_no_room_in_use				2051//当前没有房间在使用，无法加入房间
#define UNO_create_room_success			2052//创建房间成功
#define UNO_enter_room_success			2053//加入房间成功
#define UNO_someone_get_in				2054//有人进房间了
#define UNO_show_roomlist				2055//向客户端展示房间列表
#define UNO_leave_room_success			2056//离开房间成功
#define UNO_show_roomlist_end			2057//显示房间结束
#define	UNO_cannot_exit_room			2058//不在房间内，当前状态无法退出uno房间
#define	UNO_cannot_enter_room			2059//当前状态无法进入别人的uno房间!
#define	UNO_cannot_create_room			2060//当前状态无法创建uno房间!
#define UNO_cancel_enter_room			2061//取消加入其他人的房间
#define UNO_someone_leave_room			2062//有人离开房间了
#define UNO_you_become_host				2063//你成为房间主人
#define UNO_refresh_room_member			2064//刷新房间内人员列表
#define UNO_this_room_is_full			2065//当前房间已满
#define UNO_this_room_is_empty			2066//当前房间为空
#define UNO_add_full_robot_success		2067//添加完成全部机器人
#define UNO_not_got_room_operate_right	2068//没权限对房间成员进行操作
#define UNO_lack_of_players				2069//房间开启游戏人数不足
#define UNO_game_start					2070//告诉房间其他成员游戏开始了
#define UNO_room_is_in_game				2071//房间内正在游戏，无法进入
#define UNO_already_in_room				2072//你已经在房间里了
#define UNO_cannot_operate_this			2073//不能进行此次操作


//游戏内信号
#define UNO_apply_get_card				2020//申请抽一张卡
#define UNO_i_only_got_one_card			2021//我只剩一张牌了
#define	UNO_apply_pass					2022//过，让下一个人出牌
#define UNO_apply_play_this_card		2023//打出这张牌


//动画（未实现）当客户端收到类似信号时，展示动画效果
#define UNO_direct_change				2085//方向改变，顺时针或逆时针
#define	UNO_someone_be_forbidden		2086//有人被跳过了
//一般功能消息
#define UNO_refresh_all_data			2095//更新所有信息
#define	UNO_player_choose_color_change	2096//当前可以出的颜色更变
#define UNO_please_choose_color			2097//请玩家选择颜色，四选一
#define UNO_add_game_log				2098//添加日志消息

//提示
#define UNO_please_get_card				2100//当用户选择“过”时，没抽卡时提醒抽卡
#define	UNO_got_punish_choose_pass		2101//当前有加罚效果，请直接选择“过”
#define	UNO_cannot_play_this_card		2102//自己回合当前无法使用此卡
#define UNO_got_card_please_choose_pass	2103//已经抽过卡，请选择“过”
#define	UNO_got_punish					2104//当前有加罚效果!无法打出此卡

//游戏结束
#define UNO_game_over_lack_player		2110//游戏因为人员不足而结束
#define UNO_game_over_someone_win		2111//游戏因为获胜而结束






#endif