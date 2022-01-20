#ifndef INCLUDE_UNO_AI_H
#define INCLUDE_UNO_AI_H

#include "UNO_deck_process.h"

UNO_player* UNO_check_who_is_the_next(UNO_room* this_room,UNO_player* p1)//ȷ��˭���¼�
{
	int pointer;
	
	if(this_room->clock_direct)//ȷ���¼�
	{
		pointer=p1->room_member_NO+1;
		if(pointer==UNO_member_limit)pointer=0;
		UNO_player* next_player=&this_room->player[pointer];
		while(next_player->state==UNO_empty)
		{
			if(this_room->game_over)return NULL;//�����Ϸ�����ˣ����ü�������
			pointer++;
			if(pointer==UNO_member_limit)pointer=0;
			next_player=&this_room->player[pointer];
		}

	}
	else
	{
		pointer=p1->room_member_NO-1;
		if(pointer==0)pointer=UNO_member_limit-1;
		UNO_player* next_player=&this_room->player[pointer];
		while(next_player->state==UNO_empty)
		{
			if(this_room->game_over)return NULL;//�����Ϸ�����ˣ����ü�������
			pointer--;
			if(pointer==-1)pointer=UNO_member_limit-1;
			next_player=&this_room->player[pointer];
		}
	}
	return &this_room->player[pointer];
}

void UNO_AI_which_color_i_wanna_play(UNO_room* this_room,UNO_player* p1)//�����Լ����ƣ�ȷ���Լ����ʲô��ɫ�ƣ�����Լ���������ѡ�������ѡ���Ǹ���ɫ
{
	cout<<"AI˼��Ҫʲô��ɫ\n";
	int color_box[4];//4����ɫ���洢��ɫ����������Ϊ��ɫ���Ϊ1��2��3��4�����Դ浽0��1��2��3
		for(int k=0;k<4;k++)
		{
			color_box[k]=0;
		}
		int i=0;
		while(i<UNO_deck_card_limit&&p1->card_color[i]!=UNO_none)//����ǰ���۲�Ϊ�� �� û��ͷ
		{
			if(p1->card_color[i]!=UNO_black)//�����Ϊ��ɫ�����¼�Լ�����x��ɫ��������+1
			{
				color_box[p1->card_color[i]-1]++;//����ɫ����+1
			}
			i++;//����һ������
		}

		int color_number=0;//���������ʼֵΪ0
		
		p1->i_want_play_this_card=UNO_none;//���ԭ����Ҫ����ɫ
		for(i=0;i<4;i++)
			if(color_box[i]>color_number)//�����ǰ��ɫ�����������������
			{
				color_number=color_box[i];//�����������
				bool find_another=false;
				for(int j=0;j<UNO_member_limit;j++)
				{
					if(this_room->player[j].card_color[1]==UNO_none&&this_room->player[j].only_color==i+1)//�����ǰѡ����ɫ����ĳ��Ӯ���������ɫ
					{
						find_another=true;
						break;
					}
				}
				if(find_another)continue;
				p1->i_want_play_this_card=i+1;//����ǰ�������ɫ����Ϊ��ǰ��ɫ
			}

		if(p1->i_want_play_this_card==UNO_none)//���û��ȡ����Ҫ����ɫ��˵�������Լ�ѡ����ɫ�ᵼ�±���Ӯ
		{
			cout<<"AI��ʱû�ҵ���Ҫ����ɫ\n";
			for(int k=0;k<4;k++)
			{
				color_box[k]=0;
			}
			for(int i=0;i<UNO_member_limit;i++)
				if(this_room->player[i].card_color[1]==UNO_none&&this_room->player[i].only_color!=UNO_none)
				{
					color_box[this_room->player[i].only_color-1]++;
				}
			for(int i=0;i<4;i++)
				if(color_box[i]==0)
				{
					p1->i_want_play_this_card=i+1;//ѡ����û�е���ɫ���ô�Ҷ����ò��鿨
					cout<<"�Ƹ�����"<<i+1<<endl;
					return;
				}
		}
}

bool UNO_prior_use_forbidden_or_reverse_or_add_2(UNO_room* this_room,UNO_player* p1)//���ȴ����ֹ/��ת/+2�Ŀ�
{
	UNO_player* next_player=UNO_check_who_is_the_next(this_room,p1);//ȷ���¼�
	if(next_player==NULL)
	{
		if(!this_room->game_over)
			cout<<"��ô����û������Ϸ�ͷ��ؿ�ֵ?\n";
		return true;
	}
	if(this_room->current_running_effect==UNO_none)//�����ǰû�мӷ�Ч��
		if(next_player->card_color[3]==UNO_none)//����¼���������4��
			for(int i=0;i<UNO_deck_card_limit;i++)
			{
				if(this_room->current_color==p1->card_color[i]&&(p1->card_effect[i]==UNO_forbid||p1->card_effect[i]==UNO_reverse||p1->card_effect[i]==UNO_ADD_2))
				{
					UNO_play_this_card(this_room,p1,i);//���ͬɫ�Ĺ�����
					return true;
				}
				if(this_room->last_effect==p1->card_effect[i]&&(p1->card_effect[i]==UNO_forbid||p1->card_effect[i]==UNO_reverse||p1->card_effect[i]==UNO_ADD_2))
				{
					UNO_play_this_card(this_room,p1,i);//�����ͬЧ���Ĺ�����
					return true;
				}
			}

	return false;
		
}

bool UNO_AI_think_whether_use_black_card(UNO_room* this_room,UNO_player* p1)//˼���Ƿ�ʹ����������ʹ�ú���
{
	int black_card_number=0;
	int normal_card_number=0;
	for(int i=0;i<UNO_deck_card_limit;i++)//ͳ��������ƺ������Ƶ�����
	{
		if(p1->card_color[i]==UNO_none)break;
		if(p1->card_color[i]==UNO_black)
			black_card_number++;
		else
			normal_card_number++;
	}
	if(this_room->current_running_effect==UNO_none)//�����ǰû�мӷ�Ч��
		if(black_card_number!=0&&black_card_number>=normal_card_number)//������������������ƶ����ƽ����ʼ����ʹ�ú���
		{
			UNO_AI_which_color_i_wanna_play(this_room,p1);//ȷ��Ҫ��������ɫ
			for(int i=7;i>=0;i--)//�������һ����8�ţ�һ���ɫ���������ں�ɫ+4���棬����ʹ��������
				if(p1->card_color[i]==UNO_black)
				{
					UNO_play_this_card(this_room,p1,i);
					return true;
				}
		}
	return false;
}

bool UNO_AI_think_whether_use_add_4_to_next_player(UNO_room* this_room,UNO_player* p1)//�¼�ֻʣһ���ֿ��ˣ�ʹ�ú�ɫ+4
{
	UNO_player* next_player=UNO_check_who_is_the_next(this_room,p1);//ȷ���¼�
	if(next_player==NULL)
	{
		if(!this_room->game_over)
			cout<<"��ô����û������Ϸ�ͷ��ؿ�ֵ?\n";
		return true;
	}
	if(next_player->card_color[1]==UNO_none)//�����ֻʣһ�ſ���
	{
		for(int i=0;i<UNO_deck_card_limit;i++)
			if(p1->card_effect[i]==UNO_ADD_4)//�����+4
			{
				UNO_play_this_card(this_room,p1,i);
				return true;
			}
			else
				break;
	}

	return false;
}

bool UNO_AI_think_whether_change_color(UNO_room* this_room,UNO_player* p1)//˼���Ƿ��ڽ�������úڿ��ı���ɫ
{
	for(int i=0;i<UNO_member_limit;i++)
		if(this_room->player[i].card_color[1]==UNO_none&&this_room->current_color==this_room->player[i].only_color)//�����������ֻ��һ�ſ�������ӵ���ֿ��Ŀ�����ɫ�뵱ǰ��ɫһ��
		{
			for(int j=7;j>=0;j--)
				if(p1->card_color[j]==UNO_black)//���Դ���ڿ��޸���ɫ������ʹ��������
				{
					UNO_play_this_card(this_room,p1,j);
					return true;
				}
		}
	return false;
}


bool UNO_AI_careful_follow_play_card(UNO_room* this_room,UNO_player* p1)//��������
{
		for(int i=0;i<UNO_deck_card_limit;i++)
		{
			if(p1->card_color[i]==UNO_none)//��ͷ�˻�û����ɫƥ���
			{
				break;
			}

			//��ʡ�ڿ�
			if(p1->card_color[i]==UNO_black)continue;

			//����мӷ�Ч��
			//if(this_room->current_running_effect==UNO_add_card)

			//�����ǰû�мӷ�Ч������ɫ��ͬ �� Ч����ͬ
			if((this_room->current_running_effect!=UNO_add_card&&p1->card_color[i]==this_room->current_color)||p1->card_effect[i]==this_room->last_effect)//��ƥ���
			{
				for(int j=0;j<UNO_member_limit;j++)
				{
					if(this_room->player[j].card_color[1]==UNO_none&&this_room->player[j].only_color==p1->card_color[i])//�����������ֻ��һ�����ҵ�ǰҪ�����ƽ��Ǻ��Ǹ���ͬɫ����
						continue;
				}

				UNO_play_this_card(this_room,p1,i);//�������ƴ��,����ƺ������Դ��غ��л�
				return true;//�����ж���
			}
		
		}
	return false;
}



bool UNO_AI_simple_follow_play_card(UNO_room* this_room,UNO_player* p1)//��������
{
		for(int i=0;i<UNO_deck_card_limit;i++)
		{
			if(p1->card_color[i]==UNO_none)//��ͷ�˻�û����ɫƥ���
			{
				break;
			}

			//����мӷ�Ч��
			//if(this_room->current_running_effect==UNO_add_card)

			//�����ǰû�мӷ�Ч������ɫ��ͬ �� Ч����ͬ
			if((this_room->current_running_effect!=UNO_add_card&&p1->card_color[i]==this_room->current_color)||p1->card_effect[i]==this_room->last_effect)//��ƥ���
			{
				cout<<this_room->whose_turn<<"��AI��������\n";
				
				UNO_play_this_card(this_room,p1,i);//�������ƴ��,����ƺ������Դ��غ��л�
				return true;//�����ж���
			}
		
		}
	return false;
}

void UNO_AI_think(UNO_player* p1)
{
	UNO_room* this_room=&uno_room[p1->room_No];
	//�Ⱥ�׼��ָ���������������й���׼��������ʼ˼����������еȺ�
	while(!this_room->game_start)//��Ϸ��û׼���ÿ�ʼ,����еȴ�
		Sleep(100);

	bool done;//��ǰ�Ƿ�����Լ��غϵ�һ�β���
	while(true)
	{
		done=false;//����Ϊ��
		while(&this_room->player[this_room->whose_turn]!=p1)//�����ǰ�����ҵĻغ�
		{
			if(this_room->game_over)//�����ǰû����Ϸ���ˣ�����AI˼���߳�
			{
				cout<<"AI"<<p1->room_member_NO<<"exit\n";
				return;
			}
			//����ܡ�������������

			Sleep(UNO_AI_wait_time);
		}

		for(int i=0;i<25;i++)
		{
			Sleep(UNO_AI_think_time/25);//���������С�������ʱ��
			if(this_room->game_over)//�����ǰû����Ϸ���ˣ�����AI˼���߳�
			{
				cout<<"AI"<<p1->room_member_NO<<"exit\n";
				return;
			}
		}

		

		if(&this_room->player[this_room->whose_turn]!=p1)//�����ǰ�����ҵĻغ��ˣ����������ˣ��������
		{
			continue;
		}
		
		//���������������߳�
		cout<<this_room->whose_turn<<"��AI��ʼ����\n";


		//�����Լ����ƣ�ȷ���Լ����ʲô��ɫ�ƣ����֮���Լ���������ѡ�������ѡ���Ǹ���ɫ
		UNO_AI_which_color_i_wanna_play(this_room,p1);


		//����������false����ʾû����ƣ�����������true����ʾ���֮ǰ��Щ���ߵ��»������Ѿ�����ֿ���continue��ʾ������һ���ȴ�ѭ�����ȴ���һ���Լ��Ļغϵ���
		
		




		//����Լ��¼�ֻʣһ������ȷ��only_color��last_colorһ�������������ƣ�û�еĻ����Դ��+2��û�еĻ����Դ��+4��û�еĻ����������forbid��û�еĻ����������reverse
	
		//����¼�ֻʣһ�ſ������+4
		if(UNO_AI_think_whether_use_add_4_to_next_player(this_room,p1))continue;
		
		//�����������ֻʣһ������ȷ��only_color��last_colorһ�������������Ƹı���ɫ��ѡ��һ���Լ��е���ɫ���������˵�only_color����������ı���ɫ
		if(UNO_AI_think_whether_change_color(this_room,p1))continue;
		

		//����Լ��¼��ֿ�<=3��,��������ʹ�ù��ܿ�
		if(UNO_prior_use_forbidden_or_reverse_or_add_2(this_room,p1))continue;
		
		//����Լ�ֻ�ֿ�<=3���Ҷ���ͬɫ������ڿ���Ϊ�Լ���һ�ſ�����ɫ

		//���ȳ�0

		//������ͨ����
		if(UNO_AI_careful_follow_play_card(this_room,p1))continue;//����������������ж���������һ���ȴ�ѭ�����ȴ���һ���Լ��Ļغϵ���
		
		//���û����ɫ��Ч���ܸ�������Ҫ��Ҫ���������Լ������ܻ������Ȩ
		if(UNO_AI_think_whether_use_black_card(this_room,p1))continue;
		
		//ִ�е����˵��֮ǰû�Ƹ������г鿨
		UNO_user_apply_get_card(this_room,p1,1);
		Sleep(300);
		//�鿨����������ܳ����������û�ƿɳ���ѡ�񡰹���
		if(UNO_AI_careful_follow_play_card(this_room,p1))continue;//����������������ж���������һ���ȴ�ѭ�����ȴ���һ���Լ��Ļغϵ���
		//�鿨�󣬿��Լ�Ҫ��Ҫ���ڿ�����Ϊ���ܳ鵽�ڿ�
		if(UNO_AI_think_whether_use_black_card(this_room,p1))continue;
		//������˵������鿨��Ҳûͬɫ��ͬ���ƿɳ���ѡ�񡰹���
		UNO_user_apply_change_turn(this_room,p1);//�����˲�����ɣ��л���ɫ�غϣ�����мӷ�Ч�����ڸú�����ִ�мӷ�

	}


	
	
}

#endif