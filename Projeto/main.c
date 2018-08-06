#include <allegro5\allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5\allegro_primitives.h>				
#include<allegro5\allegro_font.h>
#include<allegro5/allegro_ttf.h>
#include<stdio.h> 
#include <stdlib.h>   
typedef struct rank_lista{
	int score;
	struct rank_lista *prox;
}RANK_LISTA;

enum KEYS{ UP, DOWN, SPACE, ENTER,R,S,C};
bool keys[7] = {false, false, false,false, false, false, false};
bool done = false;
const int width = 900, height = 600, FPS = 60, FPS2 = 60;;
int i=0, hp=0, score=0, start=0, muda_tempo=0, play=0, abertura_var=1;
int pos_x=0, pos_y=0, poder_x=0, poder_y=0, inimigo_x=900, inimigo_y=500;
int segundo=0,minuto=0, move_inimigo=0,pisca=115;
char nome[4];

RANK_LISTA	*score_rank=NULL; 
ALLEGRO_FONT *press_enter=NULL,*começo=NULL,*tempo=NULL, *escreve=NULL, *escreve_final=NULL,*escreve_rank=NULL;
ALLEGRO_TIMER   *timer,*timer_do_jogo;
ALLEGRO_EVENT_QUEUE *event_queue = NULL,*event_queue2 = NULL;
ALLEGRO_DISPLAY *display   = NULL;
ALLEGRO_BITMAP  *creditos  = NULL; 
ALLEGRO_BITMAP  *menu      = NULL, *melhor = NULL;
ALLEGRO_BITMAP  *estagio   = NULL, *heroi     = NULL;
ALLEGRO_BITMAP  *heroi1    = NULL, *score_ryu = NULL;
ALLEGRO_BITMAP  *inimigo   = NULL, *poder     = NULL;
ALLEGRO_SAMPLE  *abertura  = NULL, *haduken   = NULL;
ALLEGRO_SAMPLE  *gameover  = NULL, *morre     = NULL;
ALLEGRO_SAMPLE  *tema  = NULL;
FILE *arq;

void insere_score()
{
	RANK_LISTA *novo=(RANK_LISTA*)malloc(sizeof(RANK_LISTA));
	novo->score= score;
	novo->prox= score_rank;
	score_rank= novo;
}
void printa_score_rank()
{
	RANK_LISTA *aux;
	int cont=0;
	float var_y=100;
	al_clear_to_color(al_map_rgb(0, 0, 0));
	al_draw_textf(escreve_rank,al_map_rgb(255,0,0),350,0,0,"Scores ");
	for (aux=score_rank;aux!= NULL; aux=aux->prox)
	{
		al_draw_textf(escreve_rank,al_map_rgb(255,255,0),320,var_y,0,"Player %d",aux->score);
		var_y+=50;
		if(cont==7)
			break;

		cont++;
	}	
	al_flip_display();
	al_rest(0.3);
	al_stop_timer(timer);
	pisca=-100;
}
void printa_recorde()
{
	int recorde=0;
	RANK_LISTA *aux;
	for(aux=score_rank;aux!= NULL; aux=aux->prox)
	{
		if(recorde<aux->score)
		{
			recorde=aux->score;
		}
	}
	al_clear_to_color(al_map_rgb(0, 0, 0));
	al_draw_bitmap(melhor,260,80,0);
	al_draw_textf(escreve_final,al_map_rgb(255,0,0),200,400,0,"Recorde %d",recorde);
	al_flip_display();
	al_rest(0.3);
	al_stop_timer(timer);
	pisca=-100;
}
void printa_creditos()
{
	al_draw_bitmap(creditos,0,0,0);
	al_flip_display();
	al_rest(1.0);
	al_stop_timer(timer);
	pisca=-300;
}

void salva_lista_no_arquivo()
{
	int a=0,nos=0;
	RANK_LISTA *aux;
	int aux_da_lista[100];
	
	for (aux = score_rank; aux != NULL; aux=aux->prox)
	{
		aux_da_lista[a] = aux->score;
		nos++;
		a++;
	}
	fseek(arq, 0, SEEK_SET);
	for ( a = 0; a < nos; a++)
	{
		fwrite(&aux_da_lista[a],sizeof(int),1,arq);
	}
	fflush(arq);
}
void ler_arquivo()
{
	int a[20],x,j;
	fseek(arq, 0, SEEK_SET);
	x=fread (a, sizeof(int),20,arq);
	for (j=0; j < x; j++)
	{
		score=a[j];
		insere_score();
	}
}
void aparece_inimigo()
{
	al_draw_bitmap(inimigo,inimigo_x,inimigo_y,0);					
}
void tela_inicial()
{ 
	al_clear_to_color(al_map_rgb(0, 0, 0));
	al_draw_bitmap(menu, 0, 0, 0);
	al_draw_textf(press_enter,al_map_rgb(255,255,0),450,125,0,"S - Scores");
	al_draw_textf(press_enter,al_map_rgb(255,255,0),450,225,0,"R - Record ");
	al_draw_textf(press_enter,al_map_rgb(255,255,0),450,325,0,"C - Creditos");

	if(pisca>=10&&pisca<=25 || pisca>=40&&pisca<=55||pisca>=70&&pisca<=85 || pisca>=100&&pisca<=115 )
	{
		al_draw_textf(press_enter,al_map_rgb(255,255,0),260,525,0,"Press Enter");
	}else if(pisca==130)
	{
		pisca=0;
	}
	al_draw_textf(começo,al_map_rgb(255,0,0),800,10,0,"H");
	al_draw_textf(começo,al_map_rgb(255,0,0),800,80,0,"A");
	al_draw_textf(começo,al_map_rgb(255,0,0),800,150,0,"D");
	al_draw_textf(começo,al_map_rgb(255,0,0),800,220,0,"O");
	al_draw_textf(começo,al_map_rgb(255,0,0),800,290,0,"U");
	al_draw_textf(começo,al_map_rgb(255,0,0),800,360,0,"K");
	al_draw_textf(começo,al_map_rgb(255,0,0),800,430,0,"E");
	al_draw_textf(começo,al_map_rgb(255,0,0),800,500,0,"N");
	pisca++;
}
void tela_gameover()
{
	al_stop_samples();
	al_draw_bitmap(score_ryu,0,0,0);
	al_draw_textf(escreve_final,al_map_rgb(0,0,255),200,0,0,"Game Over");
	al_draw_textf(escreve_final,al_map_rgb(255,0,0),50,250,0,"Score %d",score);
	
	if(segundo<10&&minuto<10)
	{
		al_draw_textf(escreve_final,al_map_rgb(0,0,255),80,350,0,"0%d:0%d",minuto,segundo);
	}else if (segundo>=10&&minuto<10)
	{
		al_draw_textf(escreve_final,al_map_rgb(0,0,255),80,350,0,"0%d:%d",minuto,segundo);
	}else if(segundo<10&&minuto>=10) 
	{
		al_draw_textf(escreve_final,al_map_rgb(0,0,255),80,350,0,"%d:0%d",minuto,segundo);
	}else{
		al_draw_textf(escreve_final,al_map_rgb(0,0,255),80,350,0,"%d:%d",minuto,segundo);
	}

	al_play_sample ( gameover,  2.0 ,  0.0 , 1.0 , ALLEGRO_PLAYMODE_ONCE, NULL ) ;  //play audio
	insere_score();
	al_flip_display();
	al_rest(4.0);
	muda_tempo=4;
	abertura_var=1;
	pisca-=200;
}
void tela_jogo()
{
	al_draw_bitmap(estagio,0,0,0);					//exibe imagem
	al_draw_textf(escreve,al_map_rgb(255,255,255),750,0,0,"Score %d",score);
	if(segundo<10&&minuto<10)
	{
		al_draw_textf(tempo,al_map_rgb(255,255,255),500,0,0,"0%d:0%d",minuto,segundo);
	}else if (segundo>=10&&minuto<10)
	{
		al_draw_textf(tempo,al_map_rgb(255,255,255),500,0,0,"0%d:%d",minuto,segundo);
	}else if(segundo<10&&minuto>=10) 
	{
		al_draw_textf(tempo,al_map_rgb(255,255,255),500,0,0,"%d:0%d",minuto,segundo);
	}else{
		al_draw_textf(tempo,al_map_rgb(255,255,255),500,0,0,"%d:%d",minuto,segundo);
	}
	if(keys[SPACE]==true)
	{
		al_draw_bitmap(heroi,0+pos_x,0+pos_y,0);
	}else{
		al_draw_bitmap(heroi1,0+pos_x,0+pos_y,0);
	}
}
void destroi()
{
	al_destroy_bitmap(melhor);
	al_destroy_bitmap(score_ryu);
	al_destroy_bitmap(menu);							//destruindo imagem
	al_destroy_bitmap(estagio);							//destruindo imagem
	al_destroy_bitmap(heroi);							//destruindo imagem
	al_destroy_bitmap(heroi1);							//destruindo imagem
	al_destroy_bitmap(inimigo);							//destruindo imagem
	al_destroy_bitmap(poder);							//destruindo imagem
	al_destroy_sample(gameover);						//destruindo audio
	al_destroy_sample(morre);
	al_destroy_sample(haduken);
	al_destroy_sample(tema);
	al_destroy_event_queue(event_queue);				//destruindo evento
	al_destroy_display(display);						//destruindo tela
	al_destroy_sample(abertura);						//destruindo audio
}
void carrega_objts()
{
	creditos=al_load_bitmap("creditos.jpg");
	press_enter = al_load_font("bonzai.ttf",70,0);
	começo = al_load_font("HIROSHT.ttf",70,0);
	escreve_rank = al_load_font("SHOWG.ttf",50,0);
	escreve = al_load_font("SHOWG.ttf",30,0);
	escreve_final = al_load_font("SHOWG.ttf",90,0);
	tempo = al_load_font("SHOWG.ttf",30,0);

	melhor      = al_load_bitmap("Melhor.gif");				
	menu      = al_load_bitmap("Menu.jpg");				//carregando imagem
	heroi     = al_load_bitmap("Ryu.png");				//carregando imagem
	heroi1    = al_load_bitmap("Ryu0.gif");				//carregando imagem
	inimigo   = al_load_bitmap("bison.png");//carregando imagem
	score_ryu = al_load_bitmap("score.jpg");			
	poder     = al_load_bitmap("hadoken.png");			//carregando imagem
	estagio   = al_load_bitmap("fundo_chines.jpg");		//carregando imagem
	abertura  = al_load_sample("abertura.wav");			//carregando audio
	haduken   = al_load_sample("hadouken.wav");			//carregando audio
	gameover  = al_load_sample("gameover.wav");			//carregando audio
	morre	  = al_load_sample("morre.wav");			//carregando audio
	tema	  = al_load_sample("tema.wav");			
}
void inicializando_bibliotecas()
{
	al_init_primitives_addon();			//inicializando biblioteca de formas geometricas
	al_install_keyboard();				//instalando teclado
	al_init_image_addon();				//inicializando biblioteca de imagens
    al_install_audio();					//instalando biblioteca audio
	al_init_acodec_addon();				//inicializando biblioteca do mixer do audio
	al_reserve_samples(5);				//inicializando reserva de efeitos de audio
	al_init_font_addon();
	al_init_ttf_addon();
}
void criando_e_registrando_eventos()
{
	event_queue = al_create_event_queue();
	event_queue2 = al_create_event_queue();//criando evento
	al_register_event_source(event_queue, al_get_keyboard_event_source());			//registrando evento teclado
	al_register_event_source(event_queue, al_get_display_event_source(display));	//registrando fecha janela
	al_register_event_source(event_queue, al_get_timer_event_source(timer));	//registrando evento tempo			
	al_register_event_source(event_queue2, al_get_timer_event_source(timer_do_jogo));	
}
int main(void)
{	
	int muda_pos_inimigo=2;
	arq= fopen("Rank.dat","r+b");
	ler_arquivo();
	if(arq==NULL)
	arq= fopen("Rank.dat","w+b");
	
	if(!al_init())										//inicializando Allegro
		return -1;

	display = al_create_display(width, height);			//criando tela
	if(!display)										//testando tela
		return -1;
	
	al_set_window_title(display,"Hadouken 1.0");
	inicializando_bibliotecas();
	timer=al_create_timer(1.0/FPS);		//criando tratamento de tempo
	timer_do_jogo=al_create_timer(1.0);
	carrega_objts();
	criando_e_registrando_eventos();	
	al_start_timer(timer); // inicializando o tempo 
	
	while(!done)
	{
		ALLEGRO_EVENT ev;
		al_wait_for_event(event_queue, &ev);
		if(ev.type == ALLEGRO_EVENT_KEY_DOWN)
		{
			switch(ev.keyboard.keycode)
			{
				case ALLEGRO_KEY_S:
					keys[S] = true;
					break;
				case ALLEGRO_KEY_R:
					keys[R] = true;
					break;
				case ALLEGRO_KEY_UP:
					keys[UP] = true;
					break;
				case ALLEGRO_KEY_DOWN:
					keys[DOWN] = true;
					break;
				case ALLEGRO_KEY_SPACE:
					if (hp==1&&start==1)
					{
						al_play_sample ( haduken,  2.0 ,  0.0 , 1.0 ,ALLEGRO_PLAYMODE_ONCE,NULL) ;	//play audio
					}
					keys[SPACE] = true;
					poder_x=pos_x;
					poder_y=pos_y;	
					break;
				case ALLEGRO_KEY_ENTER:
					keys[ENTER] = true;
					start=1;
					play=1;
					break;
				case ALLEGRO_KEY_C:
					keys[C] = true;
					break;
				}
		}
		else if(ev.type == ALLEGRO_EVENT_KEY_UP)
		{
			switch(ev.keyboard.keycode)
			{
				case ALLEGRO_KEY_UP:
					keys[UP] = false;
					break;
				case ALLEGRO_KEY_DOWN:
					keys[DOWN] = false;
					break;
				case ALLEGRO_KEY_ESCAPE:
					done = true;
					break;
				case ALLEGRO_KEY_S:
					keys[S] = false;
					break;
				case ALLEGRO_KEY_R:
					keys[R] = false;
					break;
				case ALLEGRO_KEY_C:
					keys[C] = false;
					break;		
			}
		}
		else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
		{
			done = true;
		}
		pos_y -= keys[UP] * 5; 
		pos_y += keys[DOWN] * 5;
		if (pos_y>=10)
		{
			pos_y -= 10;
		}
		if (pos_y<=500)
		{
			pos_y += 10;
		}		
		if (keys[ENTER]==true&&start==1)
		{
			if(play==1)
			{
			al_stop_samples();
			al_play_sample ( tema,  2.0 ,  0.0 , 1.0 , ALLEGRO_PLAYMODE_ONCE , NULL ) ;	//play audio
			play=0;
			}
			if(start==1){
			al_start_timer(timer_do_jogo);
			
			if(!al_is_event_queue_empty(event_queue2))
			{
				ALLEGRO_EVENT ev2;
				al_wait_for_event(event_queue2, &ev2);
			
				if(ev2.type == ALLEGRO_EVENT_TIMER)
				{
					if(start==1)
					segundo++;

					if(segundo==60)
					{
						segundo=0;
						minuto++;
					}
				}
			}
		}
			hp=1;
			tela_jogo();
				if(ev.type == ALLEGRO_EVENT_TIMER)
				{
					if(score<100)
					{
					inimigo_x-=6;
					}else if(score>=100 && score<250)
					{
						inimigo_x -=9;
					}else if(score>=250 && score<350)
					{
						inimigo_x -=10;
					}else if(score>=350&&score<450)
					{
						inimigo_x -=11;
					}else if(score>=450&&score<650)
					{
						inimigo_x -=12;
					}else if(score>=650&&score<750)
					{
						inimigo_x -=13;
					}else if(score>=750&&score<850)
					{
						inimigo_x -=14;
					}else if(score>=850)
					{
						inimigo_x -=15;
					}

					aparece_inimigo();	
					if(inimigo_x<=pos_x)
					{
						tela_gameover();
						start=0;
						segundo=0;
						score=0;
						inimigo_x=800;
						al_flip_display();
					}
				}
			if (keys[SPACE]==true)
			{			
				al_draw_bitmap(poder,i+poder_x+70 ,poder_y+10,0);	//exibe imagem
				if(poder_x>=width)
				{
					keys[SPACE]=false;								
				}else if(( poder_x>=inimigo_x-10 && poder_x<=inimigo_x+10)&& (poder_y>=inimigo_y-30 &&poder_y<=inimigo_y+60))
				{
					score+=10;
					keys[SPACE]=false;								
					al_play_sample (morre,  1.0 ,  0.0 , 1.0 , ALLEGRO_PLAYMODE_ONCE , NULL ) ;	//play audio
					tela_jogo();							
					
					if (muda_pos_inimigo==11)
					{
						muda_pos_inimigo=2;
					}

					inimigo_y = rand()%500 +100;
					inimigo_x=800;
					if(inimigo_y<100)
					{	
						inimigo_y=100;
					}else if(inimigo_y>500)
					{
						inimigo_y=500;
					}

					if(inimigo_x<100)
						{
							inimigo_x=800;
						}
				}
			poder_x+=10;	
			muda_pos_inimigo++;
			}
		}else
		{
			if(abertura_var==1)
			al_play_sample ( abertura,  2.0 ,  0.0 , 1.0 , ALLEGRO_PLAYMODE_LOOP, NULL ) ;	
			abertura_var=0;
			if(keys[S]==true)
			{
				printa_score_rank();
				al_start_timer(timer);
			}else if(keys[R]==true)
			{
				printa_recorde();
				al_start_timer(timer);
			}else if(keys[C]==true)
			{
				printa_creditos();
				al_start_timer(timer);
			}

			tela_inicial();
			al_stop_timer(timer_do_jogo);
			start=0;	
			score=0;
			segundo=0-muda_tempo;		
		}		
		al_flip_display();		//atualiza a tela
	}

	salva_lista_no_arquivo();	
	destroi();
	free(score_rank);
	fclose(arq);
	return 0;
}