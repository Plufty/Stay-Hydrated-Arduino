//Gleidson Vinicius Gomes Barbosa - 6331
//Isaias Souza Silva 			  - 4874
//Arleson Roberto				  - 5231
/***************************************************************/
//Inclusão de bibliotecas, LCD e inclusão de CI
#include <LiquidCrystal.h>
#define pinSH_CP 11   //Pino Clock
#define pinST_CP 12  //Pino  Latch
#define pinDS    13  //Pino Data
#define qtdeCI 3

/***************************************************************/
//Configurações Componentes

LiquidCrystal lcd(0,2,3,4,5,6,7,8,9,10);

/***************************************************************/
//Estruturas

typedef struct sIntervalos//Estrutura para armazenar a quatidade de água que deve ser consumida por intervalo e o número do intervalo 
{
    int ml;
    int pos;
}Intervalos;
typedef struct sCelula
{
    Intervalos info;
    struct sCelula *next, *bef;
}Celula;

/***************************************************************/
//Variáveis
Celula *ptrlista;
Celula *aux;
Intervalos var;//Variável para o preenchimento dos intervalos.
int i, conf, cont=0, giveup=1;//Variáveis para estruturas de repetição.
int peso=0;//Variável que armazena o peso do usuário (necessário para o cálculo da quantidade de água que deverá ser consumida).
int consumo;//Variável que armazena o total de água que deve ser consumida no dia.
int intervalo;//Variável que armazena a quantidade de água a ser consumida por intervalo.
int nintervalos;//Variável que armazena o número de intervalos.
int tempo;//Variável que armazena o tamanho do intervalo.
int button=1;//Botão conectado no pino 1.
int buttons=0;//Variável para armazenar o estado do botão.
int linfinito;//Variável para o encerramento do programa.
int descontodelay=0;//Variável para desconto do delay gasto no tempo de exibição do LCD ao fim do dia.
int esperaintervalo;//Variável para o tempo de espera.

/***************************************************************/
//Assinaturas das funções

void ci74HC595Write(byte pino, bool estado);
void init (Celula **lista);
int empty (Celula *lista);
Celula* getnode();
void insereinicio(Celula **lista, Intervalos x);
void inserefim (Celula **lista, Intervalos x);
void removeinicio(Celula **lista);
void removefim(Celula **lista);

/***************************************************************/
//Funções

void ci74HC595Write(byte pino, bool estado)//Função para expansão das portas do arduino
{
	static byte ciBuffer[qtdeCI];
	bitWrite(ciBuffer[pino / 8], pino % 8, estado);
	digitalWrite(pinST_CP, LOW); //Inicia a Transmissão
	digitalWrite(pinDS, LOW);    //Apaga Tudo para Preparar Transmissão
	digitalWrite(pinSH_CP, LOW);
	for (int nC = qtdeCI-1; nC >= 0; nC--) 
	{
    	for (int nB = 7; nB >= 0; nB--) 
        {
        	digitalWrite(pinSH_CP, LOW);  //Baixa o Clock      
        	digitalWrite(pinDS,  bitRead(ciBuffer[nC], nB) );     //Escreve o BIT        
        	digitalWrite(pinSH_CP, HIGH); //Eleva o Clock
        	digitalWrite(pinDS, LOW);     //Baixa o Data para Previnir Vazamento      
    	}  
	}

	digitalWrite(pinST_CP, HIGH);  //Finaliza a Transmissão

}
void init (Celula **lista)
{
    *lista=NULL;
}
int empty (Celula *lista)
{
    if (lista==NULL)
    {
        return 1;
    }
    return 0;
}
Celula* getnode()
{
    return (Celula*) malloc (sizeof (Celula));
}
void insereinicio(Celula **lista, Intervalos x)//insere no inicio da lista.
{
    Celula *q;
    q=getnode();
    if (q != NULL)
    {
        q->info=x;
        if (empty(*lista))
        {
            q->bef=q;
            q->next=q;
            *lista=q;        
        }
        if (!empty(*lista))
        {
            q->bef=*lista;
            q->next=(*lista)->next;
        }
    }//fim do if (q != NULL)
    else
    {
        lcd.print ("ERRO: Falha ao");
    	lcd.setCursor (0,2);//Colocando o cursor na segunda linha
    	lcd.print ("Alocar!");
      	delay(5000);
      	lcd.clear();
    }
}
void inserefim (Celula **lista, Intervalos x)//insere no final da lista.
{
    Celula *q;
    q=getnode();
    if (q != NULL)
    {
        q->info=x;
        if (empty(*lista))
        {
            q->bef=q;
            q->next=q;
            *lista=q;        
        }
        if (!empty(*lista))
        {
            q->bef=(*lista)->bef;
            q->next=(*lista)->next;
            (*lista)->next=q;
            *lista=q;
        }
        
    }//fim do if (q != NULL)
    else
    {
        lcd.print ("ERRO: Falha ao");
    	lcd.setCursor (0,2);//Colocando o cursor na segunda linha
    	lcd.print ("Alocar!");
      	delay(5000);
      	lcd.clear();
    }
}
void removeinicio(Celula **lista)//remove do inicio.
{
  Celula *q;
  if (!empty(*lista))
  {
  		if ((*lista)==(*lista)->next)
        {
          	free(*lista);
          	*lista=NULL;
        }
    	else
        {
          	q=(*lista)->next;
          	(*lista)->next=q->next;
          	(q->next)->bef=*lista;
          	free (q);
        }
    	
  }//fim do if (!empty(*lista))
  else
  {
    	lcd.print ("ERRO: A lista");
    	lcd.setCursor (0,2);//Colocando o cursor na segunda linha
    	lcd.print ("esta vazia.");
      	delay(5000);
      	lcd.clear();
  }
}
void removefim(Celula **lista)//remove do final.
{
  Celula *q;
  if (!empty(*lista))
  {
  		if ((*lista)==(*lista)->next)
        {
          	free(*lista);
          	*lista=NULL;
        }
    	else
        {
          	q=*lista;
          	*lista=q->bef;
          	(*lista)->next=q->next;
          	(q->next)->bef=*lista;
          	free (q);
        }
    	
  } //fim do if (!empty(*lista))
  else
  {
    	lcd.print ("ERRO: A lista");
    	lcd.setCursor (0,2);//Colocando o cursor na segunda linha
    	lcd.print ("esta vazia.");
      	delay(5000);
      	lcd.clear();
  }
}

/***************************************************************/
void setup()
{
    pinMode (pinSH_CP, OUTPUT);
    pinMode (pinST_CP, OUTPUT);
    pinMode (pinDS, OUTPUT);
    lcd.begin(16,2);//inicia o led
    ci74HC595Write(19, HIGH);//liga o led branco que indica o funcionamento do programa
    lcd.print ("Bem vindo(a) ao");
    lcd.setCursor (0,2);//Colocando o cursor na segunda linha
    lcd.print ("Stay Hidrated!");
    delay (5000);
    descontodelay=descontodelay-5000;//Negativo para acertar o primeiro intervalo.
    lcd.clear();
    //para esse projeto, eu defini uma faixa de peso de 50 à 120kg, que pode ser facilmente alterada.
    lcd.print ("Escolha a sua");
    lcd.setCursor (0,2);//Colocando o cursor na segunda linha
    lcd.print ("faixa de peso");
    delay(5000);
    lcd.clear();
    descontodelay=descontodelay-5000;//Negativo para acertar o primeiro intervalo.
    lcd.print ("Entre 50kg e 60kg");
    delay (5000);
    descontodelay=descontodelay-5000;//Negativo para acertar o primeiro intervalo.
    lcd.clear();
    lcd.print ("Mantenha o botao");
    lcd.setCursor (0,2);//Colocando o cursor na segunda linha
    lcd.print ("pressionado 6s");
    delay (5000);
    descontodelay=descontodelay-5000;//Negativo para acertar o primeiro intervalo.
    lcd.clear();
    buttons=digitalRead(button);//Armazena o estado do botão.
    if (buttons==HIGH)
      {
          peso = 55;//Peso aproximado, o consumo recomendado é entre 30 e 35ml por kg, usando assim o calculo de peso medio*35 a aproximaão e aceitável.        
      }
    else //se o peso for maior que 60kg.
    {
		lcd.print ("Entre 60kg e 70kg");
		delay (5000);
		descontodelay=descontodelay-5000;//Negativo para acertar o primeiro intervalo.
		lcd.clear();
		lcd.print ("Mantenha o botao");
		lcd.setCursor (0,2);//Colocando o cursor na segunda linha
		lcd.print ("pressionado 6s");
		delay (5000);
		descontodelay=descontodelay-5000;//Negativo para acertar o primeiro intervalo.
		lcd.clear();
    	buttons=digitalRead(button);//Armazena o estado do botão.
		if (buttons==HIGH)
		{
				peso = 65;//Peso aproximado, o consumo recomendado é entre 30 e 35ml por kg, usando assim o calculo de peso medio*35 a aproximaão e aceitável.        
		}
	    else //se o peso for maior que 70kg.
		{
			lcd.print ("Entre 70kg e 80kg");
			delay (5000);
			descontodelay=descontodelay-5000;//Negativo para acertar o primeiro intervalo.
			lcd.clear();
			lcd.print ("Mantenha o botao");
			lcd.setCursor (0,2);//Colocando o cursor na segunda linha
			lcd.print ("pressionado 6s");
			delay (5000);
			descontodelay=descontodelay-5000;//Negativo para acertar o primeiro intervalo.
			lcd.clear();
            buttons=digitalRead(button);//Armazena o estado do botão.
			if (buttons==HIGH)
			{
				peso = 75;//Peso aproximado, o consumo recomendado é entre 30 e 35ml por kg, usando assim o calculo de peso medio*35 a aproximaão e aceitável.        
			}
			else //se o peso for maior que 80kg.
			{
				lcd.print ("Entre 80kg e 90kg");
				delay (5000);
				descontodelay=descontodelay-5000;//Negativo para acertar o primeiro intervalo.
				lcd.clear();
				lcd.print ("Mantenha o botao");
				lcd.setCursor (0,2);//Colocando o cursor na segunda linha
				lcd.print ("pressionado 6s");
				delay (5000);
				descontodelay=descontodelay-5000;//Negativo para acertar o primeiro intervalo.
				lcd.clear();
               	buttons=digitalRead(button);//Armazena o estado do botão.
				if (buttons==HIGH)
				{
					peso = 85;//Peso aproximado, o consumo recomendado é entre 30 e 35ml por kg, usando assim o calculo de peso medio*35 a aproximaão e aceitável.        
				}						
				else //se o peso for maior que 90kg.
				{
					lcd.print ("Entre 90kg e 100kg");
					delay (5000);
					descontodelay=descontodelay-5000;//Negativo para acertar o primeiro intervalo.
					lcd.clear();
					lcd.print ("Mantenha o botao");
					lcd.setCursor (0,2);//Colocando o cursor na segunda linha
					lcd.print ("pressionado 6s");
					delay (5000);
					descontodelay=descontodelay-5000;//Negativo para acertar o primeiro intervalo.
					lcd.clear();
                    buttons=digitalRead(button);//Armazena o estado do botão.
					if (buttons==HIGH)
					{
						peso = 95;//Peso aproximado, o consumo recomendado é entre 30 e 35ml por kg, usando assim o calculo de peso medio*35 a aproximaão e aceitável.        
					}
					else //se o peso for maior que 100kg.
					{
						lcd.print ("Entre 100kg e 110kg");
						delay (5000);
						descontodelay=descontodelay-5000;//Negativo para acertar o primeiro intervalo.
						lcd.clear();
						lcd.print ("Mantenha o botao");
						lcd.setCursor (0,2);//Colocando o cursor na segunda linha
						lcd.print ("pressionado 6s");
						delay (5000);
						descontodelay=descontodelay-5000;//Negativo para acertar o primeiro intervalo.
						lcd.clear();
                        buttons=digitalRead(button);//Armazena o estado do botão.
						if (buttons==HIGH)
						{
								peso = 105;//Peso aproximado, o consumo recomendado é entre 30 e 35ml por kg, usando assim o calculo de peso medio*35 a aproximaão e aceitável.        
						}
						else //se o peso for maior que 110kg.
						{
							lcd.print ("Entre 110kg e 120kg");
							delay (5000);
							descontodelay=descontodelay-5000;//Negativo para acertar o primeiro intervalo.
							lcd.clear();
							lcd.print ("Mantenha o botao");
							lcd.setCursor (0,2);//Colocando o cursor na segunda linha
							lcd.print ("pressionado 6s");
							delay (5000);
							descontodelay=descontodelay-5000;//Negativo para acertar o primeiro intervalo.
							lcd.clear();
                            buttons=digitalRead(button);//Armazena o estado do botão.
							if (buttons==HIGH)
							{
									peso = 115;//Peso aproximado, o consumo recomendado é entre 30 e 35ml por kg, usando assim o calculo de peso medio*35 a aproximaão e aceitável.        
							}
						}//fim do else (se o peso for maior que 110kg.)	      	
					}//fim do else (se o peso for maior que 100kg.)	      	
				}//fim do else (se o peso for maior que 90kg.)	      	
			}//fim do else (se o peso for maior que 80kg.)	      	
		}//fim do else (se o peso for maior que 70kg.)
    } //fim do else (se o peso for maior que 60kg.)
    consumo = peso*35;//Quantidade de água recomendada para consumir diariamente.
    nintervalos=4;//número mínimo de intervalos.
    while ((consumo/nintervalos) > 400)//O consumo recomendado por intervalo é 400 ml.(cerca de dois copos americanos)
    {
        nintervalos=nintervalos+1;
    }
    tempo=720/nintervalos;//Trabalharemos com minutos e usaremos como base o tempo de 12 horas diárias (720 minutos).
    while ((consumo%nintervalos)!= 0)//Aproximação para trabalhar apenas com números inteiros.
    {
        consumo=consumo+1;
    }
    lcd.print ("O seu consumo");
  	lcd.setCursor (0,2);//Colocando o cursor na segunda linha
    lcd.print ("devera ser de");
    delay (5000);
  	descontodelay=descontodelay-5000;//Negativo para acertar o primeiro intervalo.
  	lcd.clear();//Limpando o LCD para o proximo print
    lcd.print (consumo);//Quantidade de agua para consumir diariamente
  	lcd.print(" ml de agua");
  	lcd.setCursor (0,2);//Colocando o cursor na segunda linha
  	lcd.print ("diariamente.");
    delay (5000);
  	descontodelay=descontodelay-5000;//Negativo para acertar o primeiro intervalo.
  	lcd.clear();//Limpando o LCD para o proximo print
  	intervalo=consumo/nintervalos;//Definição da quantidade de água que deve ser consumida por intervalo.
  	lcd.print ("Seu consumo sera ");
  	lcd.setCursor (0,2);//Colocando o cursor na segunda linha
  	lcd.print ("dividido em ");
  	delay(5000);
  	descontodelay=descontodelay-5000;//Negativo para acertar o primeiro intervalo.
  	lcd.clear();//Limpando o LCD para o proximo print
  	lcd.print (nintervalos);//Número de intervalos diarios
  	lcd.print (" x diarias de ");
  	lcd.setCursor (0,2);//Colocando o cursor na segunda linha
    lcd.print (intervalo);//Quantidade de agua por intervalo
  	lcd.print (" ml de agua");
  	delay (5000);
  	descontodelay=descontodelay-5000;//Negativo para acertar o primeiro intervalo.
  	lcd.clear();//Limpando o LCD para o proximo print
  	lcd.print ("divididas em");
  	lcd.setCursor (0,2);//colocando o cursor na segunda linha
  	lcd.print ("intervalos de");
  	delay (5000);
  	descontodelay=descontodelay-5000;//Negativo para acertar o primeiro intervalo.
  	lcd.clear();//Limpando o LCD para o proximo print
  	lcd.print (tempo);//Tempo entre cada intervalo
  	lcd.print (" minutos.");
	delay(5000);
  	descontodelay=descontodelay-5000;//Negativo para acertar o primeiro intervalo.
  	lcd.clear();//Limpando o LCD para o proximo print
    for (i=0; i<nintervalos; i++)//Alocação das células da lista.
    {
        var.ml=intervalo;
        var.pos=i+1;   
      	if (ptrlista == NULL)
        {
          	insereinicio(&ptrlista,var);
        }
      	else
        {
        	inserefim (&ptrlista, var);
        } 
    }
}

void loop()
{
    while (giveup != 0)
    {
   	    ci74HC595Write(19, HIGH);
		aux=ptrlista;
    	while (cont < nintervalos)
    	{
	        aux=aux->next;
          	ci74HC595Write(23, HIGH);//O Buzzer emite som quando for hora de se hidratar.
	      	ci74HC595Write(20, HIGH);//LED fica verde durante o tempo de se hidratar.
	        lcd.print ("Hora de se");
	        lcd.setCursor (0,2);//colocando o cursor na segunda linha
	        lcd.print ("hidratar!!");
	      	delay (5000);
          	descontodelay=descontodelay+5000;
	      	lcd.clear();//Limpando o LCD para o proximo print
	        lcd.print ("Intervalo ");
	      	lcd.print ((aux->info).pos);
            lcd.print ("/");
            lcd.print (nintervalos);
	        lcd.setCursor (0,2);//colocando o cursor na segunda linha
	      	lcd.print ("consuma ");
	      	lcd.print (intervalo);
	      	lcd.print ("ml.");
	      	delay (5000);
          	descontodelay=descontodelay+5000;
	      	lcd.clear();//Limpando o LCD para o proximo print
            lcd.print ("Espere 40s para");
          	lcd.setCursor (0,2);//colocando o cursor na segunda linha
	      	lcd.print ("adiar, ou ");
	      	delay (5000);            
          	descontodelay=descontodelay+5000;
          	lcd.clear();//Limpando o LCD para o proximo print
	        lcd.print ("para confirmar");
	      	lcd.setCursor (0,2);//colocando o cursor na segunda linha
	      	lcd.print ("o consumo");
	      	delay (5000);
          	descontodelay=descontodelay+5000;
          	lcd.clear();//Limpando o LCD para o proximo print
            lcd.print ("mantenha o botao");
	      	lcd.setCursor (0,2);//colocando o cursor na segunda linha
	      	lcd.print ("pressionado 6s.");
	      	delay (5000);
          	descontodelay=descontodelay+5000;
	      	lcd.clear();//Limpando o LCD para o proximo print
          	ci74HC595Write(23, LOW);//Desliga o buzzer
          	buttons=digitalRead(button);//Armazena o estado do botão.
          	if (buttons==HIGH)
            {
               lcd.print ("Para consumo de");
               lcd.setCursor (0,2);//colocando o cursor na segunda linha
               lcd.print (intervalo);
               lcd.print ("ml espere 30s");
               delay (5000);
               descontodelay=descontodelay+5000;
               lcd.clear();//Limpando o LCD para o proximo print
               lcd.print ("Para consumo de");
               lcd.setCursor (0,2);//colocando o cursor na segunda linha
               lcd.print (intervalo*2);
               lcd.print ("ml de agua");
               delay (5000);
               descontodelay=descontodelay+5000;
               lcd.clear();
    	       lcd.print ("Mantenha o botao");
	   		   lcd.setCursor (0,2);//colocando o cursor na segunda linha
	      	   lcd.print ("pressionado 6s");
	      	   delay (5000);
               descontodelay=descontodelay+5000;
               lcd.clear();
               buttons=digitalRead(button);//Armazena o estado do botão.
               if (buttons==HIGH)
               {
                  conf=2;//para o consumo de água equivalente a 2 intervalos
               }               
               else
               {
               	  delay (30000);//delay 30s para consumo de 1 intervalo.
                  descontodelay=descontodelay+30000;
                  conf=1;//para o consumo de água equivalente a 1 intervalo
               }
            }//fim do if (buttons==HIGH)
          	else
            {
              	conf==3;//para pular o consumo de água do intervalo citado.
                delay (40000);//delay 40s apra adiar o consumo.
                descontodelay=descontodelay+40000;
            }
	      	ci74HC595Write(20, LOW);
            ci74HC595Write(23, LOW);
	        if (conf != 1 && conf != 2)//Se o intervalo tiver sido adiado ou perdido
	        {
                ci74HC595Write(20, HIGH);//LED fica amarelo durante um intervalo perdido ou adiado.
                ci74HC595Write(22, HIGH);//Combinação de duas cores para gerar o led amarelo.
                lcd.print ("O consumo deste");
                lcd.setCursor (0,2);//colocando o cursor na segunda linha  
                lcd.print ("intervalo foi");
              	delay(5000);
              	descontodelay=descontodelay+5000;
              	lcd.clear();
              	lcd.print("perdido ou");
                lcd.setCursor (0,2);//colocando o cursor na segunda linha
				lcd.print ("adiado por vc.");
              	delay(5000);
              	descontodelay=descontodelay+5000;
              	lcd.clear();
              	delay (5000); 
              	descontodelay=descontodelay+5000;
                ci74HC595Write(20, LOW);
              	ci74HC595Write(22, LOW);
	            aux=aux->bef;
	            cont=cont-1;
	        }//fim do if (conf != 1 && conf != 2)
	        if (cont != nintervalos-1)//Se não for o último consumo
	        {
		        if (conf == 2)
	            {
	                aux=aux->next;
	                cont=cont+1;
	            }
	        }
	        if (cont == nintervalos-1)//Se for o último intervalo
	        {
		        if (conf == 2)
	            {	             
                 	conf=1; 
                  	lcd.print("Restam apenas +");
                  	lcd.setCursor (0,2);//colocando o cursor na segunda linha
                  	lcd.print ("400 ml para hoje."); 
                  	delay (5000);
                  	descontodelay=descontodelay+5000;
                  	lcd.clear();
	            }
	        }
            if (conf == 1 || conf == 2)
            {
            	ci74HC595Write(21, HIGH);//LED fica azul durante um intervalo no qual a água foi consumida.
              	if (conf == 1)
                {
                    lcd.print ("Vc consumiu ");
                    lcd.print (intervalo);
                    lcd.setCursor (0,2);//colocando o cursor na segunda linha
                  	lcd.print ("ml de agua");
                  	delay (5000);
                  	descontodelay=descontodelay+5000;
                  	lcd.clear();
                }//fim do if (conf == 1)
              	if (conf == 2)
                {
                  	lcd.print ("Vc consumiu ");
                  	lcd.print (intervalo*2);
                  	lcd.setCursor (0,2);//colocando o cursor na segunda linha
                  	lcd.print ("ml de agua");
                  	delay (5000);
                  	descontodelay=descontodelay+5000;
                  	lcd.clear();
                }//fim do if (conf == 2)
              
                delay(500);//apenas pára fins de teste
	        	ci74HC595Write(21, LOW);
            }  
	        cont = cont+1;
          	esperaintervalo=(tempo*60*1000)-descontodelay;
          	delay (esperaintervalo);
          	descontodelay=0;//Zerando o desconto para o próximo intervalo ou desconto ao fim do dia.
      	
	    }
      	ci74HC595Write(22, HIGH);//LED fica vermelho no intervalo entre o final do consumo diario e o inicio do novo dia.
	    lcd.print ("Parabens, hoje ");
      	lcd.setCursor (0,2);//colocando o cursor na segunda linha
      	lcd.print ("vc consumiu ");
      	delay(5000);
      	descontodelay=descontodelay+5000;
      	lcd.clear();
      	lcd.print (consumo);
      	lcd.print (" ml de agua");
      	delay (5000);
      	descontodelay=descontodelay+5000;
      	lcd.clear();
	    lcd.print ("Ate amanha!");
        delay(5000);
      	descontodelay=descontodelay+5000;
      	lcd.clear();
      	lcd.print ("Em 12h seu prox");
      	lcd.setCursor (0,2);//colocando o cursor na segunda linha
      	lcd.print ("dia se iniciara.");
      	delay (5000);
      	descontodelay=descontodelay+5000;
      	lcd.clear();      	
	    lcd.print ("Para encerrar");
      	delay (5000);
      	descontodelay=descontodelay+5000;
      	lcd.clear();
      	lcd.print ("mantenha o botao");
      	lcd.setCursor (0,2);//colocando o cursor na segunda linha
      	lcd.print ("pressionado.");
      	delay (5000);
      	descontodelay=descontodelay+5000;
      	lcd.clear();
      	buttons=digitalRead(button);//Armazena o estado do botão.
        if (buttons==HIGH)//Se o programa for encerrado
        {
          	ci74HC595Write(19, LOW);
            linfinito=1;
            lcd.print ("O programa sera");
          	lcd.setCursor (0,2);//colocando o cursor na segunda linha
          	lcd.print ("encerrado.");
            delay(5000);
            lcd.clear();          
          	while (ptrlista != NULL)
            {
              	removeinicio(&ptrlista);
              	removefim(&ptrlista);//dois remove para questão de otimização
            }
          	while (linfinito==1)//Loop Infinito até desligar a alimentação.
            {
            }
        }//fim do if (buttons==HIGH)
      	lcd.print ("Para continuar");
      	lcd.setCursor (0,2);//colocando o cursor na segunda linha
      	lcd.print ("aguarde o inicio");
        delay (5000);
      	descontodelay=descontodelay+5000;
      	lcd.clear();
      	lcd.print ("do novo dia.");
      	delay (5000);
      	descontodelay=descontodelay+5000;
      	lcd.clear();
      	delay (43200000-descontodelay);//12h - o delay gasto para impressão no LCD até o próximo dia.
      	buttons=digitalRead(button);//Armazena o estado do botão.
        if (buttons==LOW)//Se o programa não for encerrado.
        {           
            cont=0;
        }
        else//Para o caso de decisão de encerrar o programa antes do início do novo dia
        {
          	ci74HC595Write(19, LOW);
            linfinito=1;
            lcd.print ("O programa sera");
          	lcd.setCursor (0,2);//colocando o cursor na segunda linha
          	lcd.print ("encerrado.");
            delay(5000);
            lcd.clear();
          	while (ptrlista != NULL)//limpa toda a lista.
            {
              	removeinicio(&ptrlista);
                if (ptrlista!=NULL)//se não estiver vazio, remove duas vezes para desenfileirar mais rápido.
                {
              	   removefim(&ptrlista);
                }
            }
          	while (linfinito==1)//Loop Infinito até desligar a alimentação.
            {
            }
        }
      	ci74HC595Write(22, LOW);//Led vermelho se apaga.
    }
}
