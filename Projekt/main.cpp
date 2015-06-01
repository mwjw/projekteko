//---------------------------------------------------------------------------


#include <vcl.h>
#include <list>
#include <iostream>
#include <fstream>
#include <string>

#include <math>
#pragma hdrstop

#include "main.h"
  using namespace std;
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfMain *fMain;
TCheckBox *checkbox;
AnsiString temp,temp2;
AnsiString weryfikacja;

#define MAX_ZM 13
#define MAX_X 12
#define MAXK 4096

matrix DaneStatyst;     // dane do projektu kolumna 0 Y 1-n Xi
matrix wartSrednie;     // wiersz 0 i-kolumna
matrix macierzKorelacji;// kolumna 0 R0 1-i R


double integralnaPojemnosc[MAXK]; //
int ileZmiennych;

int kombinacja[MAX_X];
int bestKomb[MAX_X];
// wspolczynki L dla rownania modelu indeksowne od 0
double pararametryModelu[MAX_X];
int ileKomb;

        double Su2; // wariancja skladnika resztowego
double Su; // odchylenie standardowe skladnika resztowego
double Da[MAXK];//bledy srednie szacunku parametrow
double Fi2; // wspolczynik zbieznosci
double V; // wspolczynnik zmiennosci losowej V
  bool wyswietl = false;
// deklaracje funkcji
bool wczytajPlik(String nazwa);// funkcja wczytujaca dane z pliku tekstowego
void obliczWartSrednie();      // wartosci srednie
/////
int wybranych;
int wybraneZmienne[4];
float rGwiazdka;
bool zaokraglaj = false;
double R[4][4];
double R0[4][1];
double W[5][5];
double Rprim[4][4];
double Y[10][1];
unsigned char wierzcholki[4][4];
unsigned char sciezki[16][4];
unsigned char cWierzcholki=0, cSciezki=0;
/////
   int next_comb(int comb[], int k, int n);

//---------------------------------------------------------------------------
__fastcall TfMain::TfMain(TComponent* Owner)
        : TForm(Owner)
{

   
}
//---------------------------------------------------------------------------
double srednia(double tablica[10][5], int nrKolumny)
{
    double suma = 0;
    for(int i=0; i<10; i++)
    {
        suma += tablica[i][nrKolumny];
    }
    return suma/10;
}
double zaokr(double liczba, char format[5])
{
    char temp[20];
    sprintf(temp, format, liczba);
    return strtod(temp, NULL);
}
int max(int t[], int n)
{
    int max = t[0];
    for(int i = 1; i < n; i++)
            if(t[i] > max) max = t[i];

    return max;
}
///////////////////////
void __fastcall TfMain::bZmienClick(TObject *Sender)
{
// zmieniam zrodlo danych i probuje wczytac
this->dZmienPlik->Execute();
this->bMacierzKorelacji->Enabled = FALSE;

this->bMNK->Enabled = FALSE;


if (wczytajPlik(this->dZmienPlik->FileName)){
        this->mOutput->Lines->Clear();
        temp = temp.sprintf("Wczytalem  dane z pliku %s",this->dZmienPlik->FileName.c_str());
        this->mOutput->Lines->Append(temp);

        temp = "\t Y\t";
        int i;
        for (i=1;i<DaneStatyst.cols();i++)
        temp += temp2.sprintf("X%d\t",i);

//        this->mOutput->Lines->Append(temp);
        this->mOutput->Lines->Append(DaneStatyst.wypisz(temp));
        this->bMacierzKorelacji->Enabled=TRUE;

        temp = temp2.sprintf("\r\nLiczba probek %d",DaneStatyst.rows());
            temp += temp2.sprintf("\r\n1 zmienna objasniania Y i %d zmienne objasniajce X",DaneStatyst.cols()-1);
            this->mOutput->Lines->Append(temp);
}
else   {
       // ShowMessage("Nie mogê wczytac pliku dane.txt. Wybierz \"Zmieñ Dane\"");
    this->mOutput->Lines->Clear();
                      temp = temp.sprintf("Nie mogê wczytac pliku %s Wybierz \"Zmieñ Dane\"",this->dZmienPlik->FileName.c_str());
    this->mOutput->Lines->Append(temp);
        }   

}

bool wczytajPlik(String nazwa)
{
          fstream plik;

           plik.open(nazwa.c_str(),ios::in);
           string c,c3;
           int linia=0;
           int w=0;
             char bufor[1024];
if (!plik.fail()){

        while(plik && linia<MAX_ZM){

	getline(plik,c);


       
	if (c.size()>10)
	{
		string c2;

		unsigned int i = 0;

                w=0;
		while(i<c.size()){

		do{
			if(c[i]!= ' ' && c[i]!='\r'&& c[i]!='\t' && c[i]!='\n' && (c[i]=='.' || c[i]>='0' && c[i]<='9') )
			    {
                                if (c[i]=='.')
                                c2+=',';
                                else
                            	c2+=c[i];

                            }
                        
                     
			i++;
		}while(c[i]!=' ' && i<c.size());

                         if (c2.size()>0){

                         
                                 temp = c2.c_str();
                                  double liczba = -1;
                       try {
              liczba = temp.ToDouble();
                            }
                            catch (Exception *e)
                            {
                            ShowMessage(e->Message);
                            }
                             DaneStatyst.setwart(liczba,linia,w);

                     w++;

                    

		c2.clear();
                }
		}

        linia++;


        
        
      
         c.clear();

         }
    

	}

        DaneStatyst.setCols(w);
        DaneStatyst.setRows(linia);
        return true;
      }
    else{
  return false;
        }
}
//---------------------------------------------------------------------------
void __fastcall TfMain::FormCreate(TObject *Sender)
{
    if (wczytajPlik("dane.txt")){
        // wypisz
        this->mOutput->Lines->Clear();
        this->mOutput->Lines->Append("Wczytalem domyslne dane z pliku dane.txt");

        temp = "\t Y\t";
        int i;
        for (i=1;i<DaneStatyst.cols();i++)
        temp += temp2.sprintf("X%d\t",i);

//        this->mOutput->Lines->Append(temp);
        this->mOutput->Lines->Append(DaneStatyst.wypisz(temp));

            this->bMacierzKorelacji->Enabled = TRUE;


            temp = temp2.sprintf("\r\nLiczba probek %d",DaneStatyst.rows());
            temp += temp2.sprintf("\r\n1 zmienna objasniania Y i %d zmienne objasniajace X",DaneStatyst.cols()-1);
            this->mOutput->Lines->Append(temp);
        }
        else {
       // ShowMessage("Nie mogê wczytac pliku dane.txt. Wybierz \"Zmieñ Dane\"");
    this->mOutput->Lines->Clear();
    this->mOutput->Lines->Append("Nie mogê wczytac pliku dane.txt. Wybierz \"Zmieñ Dane\"");
        }
}


 void __fastcall TfMain::wypisz(AnsiString nazwa,matrix m){

 this->mOutput->Lines->Append(m.wypisz(nazwa));
 }
//---------------------------------------------------------------------------
void __fastcall TfMain::bMacierzKorelacjiClick(TObject *Sender)
{
// Macierz Korelacji R0 i R kolumna 0 - R0. kolumny 1-n R

int i,j,k;

i=0;
  this->mOutput->Lines->Clear();
        this->mOutput->Lines->Append("Niezbêdne wspólczyniki korelacji");

     obliczWartSrednie();
//this->mOutput->Lines->Append(wartSrednie.wypisz("Wartosci srednie"));

 k = 0;

           macierzKorelacji.setCols(DaneStatyst.cols()) ;
           macierzKorelacji.setRows(DaneStatyst.cols()-1) ;
 for(k=1;k<DaneStatyst.cols();k++)
        for (i=0;i<DaneStatyst.cols();i++)
        {
             double wynik;
             double num,dem;
             double a,b,c,d,s1,s2;
                       num = 0;
                       dem = 0;
                       s1=0;
                       s2=0;

             for (j=0;j<DaneStatyst.rows();j++)
             {
                           a = DaneStatyst.wart(j,k)- wartSrednie.wart(0,k);
                           b = DaneStatyst.wart(j,i) - wartSrednie.wart(0,i);
                           num += a*b;

                           c = (double)pow(DaneStatyst.wart(j,k) - wartSrednie.wart(0,k),2);
                           d = (double) pow(DaneStatyst.wart(j,i) - wartSrednie.wart(0,i),2);
                              s1 +=c;
                              s2 +=d;

             }
                dem =    sqrt(s1*s2);
             wynik = num / dem;
             macierzKorelacji.setwart(wynik,k-1,i);



}

     this->mOutput->Lines->Append(macierzKorelacji.wypisz("R0",macierzKorelacji.rows(),1));
       this->mOutput->Lines->Append(macierzKorelacji.wypisz("R",0,macierzKorelacji.rows(),1,macierzKorelacji.cols()));
       this->bGrafy->Enabled = TRUE;
}

void obliczWartSrednie(){

        int i,j;

        double suma;

        wartSrednie.setRows(1);
        wartSrednie.setCols(DaneStatyst.cols());
        for (i=0;i<DaneStatyst.cols();i++)
        {
        suma = 0;
         for (j=0;j<DaneStatyst.rows();j++)
         suma += DaneStatyst.wart(j,i); // sumuje kolumne

         wartSrednie.setwart(suma/DaneStatyst.rows(),0,i);



        }


}


//-----------------------------------------------------------------------------





void __fastcall TfMain::bZakonczClick(TObject *Sender)
{
this->Close();
}
//---------------------------------------------------------------------------

void __fastcall TfMain::bMNKClick(TObject *Sender)
{
// metoda najmniejszych kwadratow
AnsiString t,t2;
     int i,j,k,l;

this->mOutput->Clear();
this->mOutput->Lines->Append("METODA NAJMNIEJSZYCH KWADRATÓW - OBLICZANIE PARAMETRÓW");
this->mOutput->Lines->Add("");
this->mOutput->Lines->Add("");
this->mOutput->Lines->Append("Ogólny wzór: ");

        t = "y = ";
        j = 1;
      for (i=1;i<ileZmiennych;i++)
      {
      if (bestKomb[i]!=-1)
      {
        t += t2.sprintf("L%d X%d + ",j,bestKomb[i]+1 );
        this->mOutput->Lines->Add("--------------");
        this->mOutput->Lines->Add(bestKomb[i] );
        j++;
      }

      }

       t+=t2.sprintf("L%d",j);

      this->mOutput->Lines->Append(t);
matrix x;
matrix y;
matrix xt;
matrix xtx;
matrix xty;
matrix xtxo;
matrix A;
        int ileProbek = DaneStatyst.rows();

       x.setCols(j);        // policzone wyzej przy okazji wypisywania
       x.setRows(ileProbek);


        y.setCols(1);
        y.setRows(ileProbek);

        for (i=0;i<ileProbek;i++){
              y.setwart(DaneStatyst.wart(i,0),i,0);
              }
         if(wyswietl)
        this->mOutput->Lines->Append(y.wypisz("Y ="));





        k=0;
        for (i=1;i<ileZmiennych;i++)
      {
      if (bestKomb[i]!=-1)
      {
       l = bestKomb[i];      

       for (j=0;j<ileProbek;j++)
       {
           x.setwart(DaneStatyst.wart(j,l),j,k);

       }

       k++;
      }

      }
        // k++;
       for (j=0;j<ileProbek;j++)
       {
           x.setwart(1,j,k);// j-wiersz , k-kolumna

       }
       if(wyswietl)
          this->mOutput->Lines->Append(x.wypisz("X ="));



            xt = x.transpose();
            if(wyswietl)
          this->mOutput->Lines->Append(xt.wypisz("XT ="));


      xtx = xt*x;
      if(wyswietl)
          this->mOutput->Lines->Append(xtx.wypisz("XTX ="));







      xty = xt*y;
      if(wyswietl)
            this->mOutput->Lines->Append(xty.wypisz("XTY ="));



       xtxo = xtx.inverse();
       if(wyswietl)
        this->mOutput->Lines->Append(xtxo.wypisz("Macierz odwrotna (XTX)-1 ="));


                 A = xtxo*xty;
           this->mOutput->Lines->Append(A.wypisz("A = "));
         for (i=0;i<MAX_X;i++)
                  pararametryModelu[i]=-1;

        for (i = 0;i<A.rows();i++){
                for (j=0;j<A.cols();j++){

                pararametryModelu[i] = A.wart(i,j);
                }
        }


       t = "Ostateczny wzor \r\n y = ";
        j = 0;
      for (i=1;i<ileZmiennych;i++)
      {
      if (bestKomb[i]!=-1)
      {

         if (j!=0)
                        if ( pararametryModelu[j]>0) t+=t2.sprintf("+ ");

        t += t2.sprintf("%.4f X%d ",pararametryModelu[j],bestKomb[i]+1);
        j++;
      }

      }

            if ( pararametryModelu[j]>0) t+=t2.sprintf("+ ");
       t+=t2.sprintf("%.4f",pararametryModelu[j]);

           this->mOutput->Lines->Append(t);



      /*
      obliczenie do weryfikacji modelu, wyswietlenie po nacisnieciu przycisku "bWeryfikacja"
        double Su2; // wariancja skladnika resztowego
double Su; // odchylenie standardowe skladnika resztowego
double Da[MAXK];//bledy srednie szacunku parametrow
double Fi2; // wspolczynik zbieznosci
double V; // wspolczynnik zmiennosci losowej V
      */


      int N,K;  // N - liczbza obserwacji , K - liczba parametrow
         N  = ileProbek;
         K = j+1;


        matrix yty,yt,ytxa,D2a,fi2;
        double temp;
        yt = y.transpose();
        yty = y.transpose()*y;

        t = t2.sprintf("YTY = %.4f",yty.wart(0,0));

        ytxa = y.transpose()*x*A;
        t += t2.sprintf("\r\nYTXA = %.4f",ytxa.wart(0,0));

         temp = yty.wart(0,0) - ytxa.wart(0,0);

         t += t2.sprintf("\r\nYTY - YTXA = %.4f",temp);
         this->mOutput->Lines->Append(t);


         Su2 = (1/(double)(N-K))*temp;

         t = t2.sprintf("\r\nWariancja sk³adnika resztowego Su2 = %f",Su2);
         Su = sqrt(Su2);
t += t2.sprintf("\r\nOdchylenie standartowe sk³adnika resztowego Su = %f",Su);

       // XTX = X.transpose()*X;
          D2a = xtx.inverse();

        //  t+=t2.sprintf("\r\nD2(a):\r\n");
         for (j=0;j<D2a.cols();j++){
          for (i=0;i<D2a.rows();i++)
               {
          //     t += t2.sprintf("\t%.4f",Su2*D2a.wart(i,j));
               if (j==i) Da[i] = sqrt(Su2*D2a.wart(i,j));
               }
            //   t+=t2.sprintf("\r\n");
               }



                t+=t2.sprintf("\r\nB³êdy srednie szacunku parametów D(a):");
                for (i=0;i<D2a.cols();i++)
                {
                t+=t2.sprintf(" %.4f",Da[i]);
                }


          double sredniaY;

              sredniaY = 0;

              for (i=0;i<y.rows();i++)
              {
              sredniaY += y.wart(i,0);
              }

              sredniaY /= y.rows();

              t += t2.sprintf("\r\nWartosc srednia Y = %.4f",sredniaY);

              fi2 = (yty-yt*x*A);
              Fi2 = fi2.wart(0,0);
              fi2=((y-sredniaY).transpose()*(y-sredniaY));

              Fi2 /= fi2.wart(0,0);


               t += t2.sprintf("\r\nWspó³czynik zbie¿nosci Fi2 = %.4f",Fi2);
                t += t2.sprintf("\r\nWspó³czynik determinacji R2 = 1- Fi2 = %.4f",1-Fi2);
                t += t2.sprintf("\r\nWspó³czynik zmiennosci losowej V = %.4f %",(Su/sredniaY)*100);

         weryfikacja = t;
           this->mOutput->Lines->Append(t);

   
}
//---------------------------------------------------------------------------

void __fastcall TfMain::bWeryfikacjaClick(TObject *Sender)
{
this->mOutput->Lines->Clear();
this->mOutput->Lines->Append("Weryfikacja istotnosci dobranych parametrów modelu ekonometrycznego");
this->mOutput->Lines->Append(weryfikacja);

int i,j;
           AnsiString t,t2;
     t = "Ostateczny wzor \r\n\ty = ";
        j = 0;
      for (i=1;i<ileZmiennych;i++)
      {
      if (bestKomb[i]!=-1)
      {

         if (j!=0)
                        if ( pararametryModelu[j]>0) t+=t2.sprintf("+ ");

        t += t2.sprintf("%.4f X%d ",pararametryModelu[j],bestKomb[i]+1);
        j++;
      }

      }

            if ( pararametryModelu[j]>0) t+=t2.sprintf("+ ");
       t+=t2.sprintf("%.4f",pararametryModelu[j]);

       t+=t2.sprintf("\r\n\t       ");
       for (i=0;i<=j;i++)
              t+=t2.sprintf("(%.4f)   ",Da[i]);

      this->mOutput->Lines->Append(t);
}
//---------------------------------------------------------------------------
TForm *okno = new TForm(Application);
TPanel *panel = new TPanel(okno);
TRadioButton *wybor = new TRadioButton(panel);
TRadioButton *wybor2 = new TRadioButton(panel);
void __fastcall TfMain::zatwierdz(TObject *Sender)
{
        if(wybor->Checked==false && wybor2->Checked==false)
    {
        ShowMessage("Wska¿ poziom istotnosci!");
    }
    else
    { 
        for(int i = 0; i<4; i++)
        {
                R0[i][0] = macierzKorelacji.wart(i,0);
                for(int j=0; j<4; j++)
                {
                        R[i][j] = macierzKorelacji(i,j+1);
                }
        }
        okno->Close();
        mOutput->Clear();
        float alfa;
        unsigned char n = 10; //liczba stopni swobody = 8
        float tAlfa;
        if(wybor->Checked)
        {
            mOutput->Lines->Add("Poziom istotnosci 0.1");
            alfa = 0.1;
            tAlfa = 1.860;
        }
       // else
        //{
          //  mOutput->Lines->Add("Poziom istotnosci 0.05");
           // alfa = 0.05;
            //tAlfa = 2.306;
        //}
        rGwiazdka = sqrt(pow(tAlfa, 2)/(n-2+pow(tAlfa, 2)));
        mOutput->Lines->Add("");
        mOutput->SelText = "r*=";
        mOutput->SelText = rGwiazdka;

        mOutput->Lines->Add("");
        mOutput->Lines->Add("");
        mOutput->Lines->Append("Macierz W");
        char buff[20];
        for(int i=0; i<5; i++)
        {
            mOutput->Lines->Add("");
            for(int j=0; j<5; j++)
            {
                if((i==0 && j==0)||(i==4 && j==4))
                {
                    W[i][j] = 1;
                }
                else if(i==0 && j>0)
                {
                    W[i][j] = R0[j-1][0];
                }
                else if(j==0)
                {
                    W[i][j] = R0[i-1][0];
                }
                else
                {
                    W[i][j] = R[i-1][j-1];
                }
                sprintf(buff, "%.3f", W[i][j]);
                mOutput->SelText = buff;
                if(j!=4)
                {
                    mOutput->SelText = " ";
                }
                if(i>0 && j>0)//&& i<=j
                {
                    if(i==j)
                    {
                        Rprim[i-1][j-1] = 1;
                    }
                    else
                    {
                        if(fabs(W[i][j])<=rGwiazdka)
                        {
                            Rprim[i-1][j-1] = 0;
                        }
                        else
                        {
                            Rprim[i-1][j-1] = fabs(W[i][j]);
                        }
                    }
                }
            }
        }
        //wyswietlenie Rprim
        mOutput->Lines->Add("");
        mOutput->Lines->Add("");
        mOutput->Lines->Append("Macierz R'");
        for(int i=0; i<4; i++)
        {
            mOutput->Lines->Add("");
            for(int j=0; j<4; j++)
            {
                sprintf(buff, "%.3f", Rprim[i][j]);
                mOutput->SelText = buff;
                if(j!=3)
                {
                    mOutput->SelText = " ";
                }
            }
        }
        //grafy
        PaintBox->Canvas->Font->Name = "Courier New CE";
        PaintBox->Canvas->Font->Size = 8;

        int liczbaSciezek[4];
        int nrGrafu[4];
        for(int i=0; i<4; i++)
        {
            nrGrafu[i] = 0;
            liczbaSciezek[i] = 0;
        }
        unsigned char x1,x2,y1,y2;
        cWierzcholki=0;
        cSciezki=0;
        unsigned char nrGrafuC = 1;
        for(int i=0; i<4; i++)
        {
            for(int j=0; j<4; j++)
            {
                if(Rprim[i][j]>0)
                {
                    if(i==j)
                    {
                        if(i%2==0)
                        {
                            x1 = 0;
                            x2 = 30;
                        }
                        else
                        {
                            x1 = 100;
                            x2 = 130;
                        }
                        y1 = i/2*70;
                        y2 = y1+30;
                        wierzcholki[cWierzcholki][0] = x1;
                        wierzcholki[cWierzcholki][1] = y1;
                        wierzcholki[cWierzcholki][2] = x2;
                        wierzcholki[cWierzcholki][3] = y2;
                        cWierzcholki++;
                    }
                    else
                    {
                        if(i%2==0)
                        {
                            x1 = 0;
                        }
                        else
                        {
                            x1 = 100;
                        }
                        if(j%2==0)
                        {
                            x2 = 0;
                        }
                        else
                        {
                            x2 = 100;
                        }
                        y1 = i/2*70;
                        y2 = j/2*70;
                        sciezki[cSciezki][0] = x1;
                        sciezki[cSciezki][1] = y1;
                        sciezki[cSciezki][2] = x2;
                        sciezki[cSciezki][3] = y2;
                        cSciezki++;
                        liczbaSciezek[i]++;
                        if(i==0)
                        {
                            nrGrafu[i] = nrGrafuC;
                        }
                        else
                        {
                            if(nrGrafu[i]==0)
                            {
                                if(nrGrafu[j]!=0)
                                {
                                    nrGrafu[i] = nrGrafu[j];
                                }
                                else
                                {
                                    nrGrafuC++;
                                    nrGrafu[i] = nrGrafuC;
                                }
                            }
                            else
                            {
                                if(nrGrafu[i]!=nrGrafu[j])
                                {
                                    nrGrafu[j] = nrGrafu[i];
                                }
                            }
                        }
                    }
                }
            }
        }
        PaintBox->Refresh();

        //wybor zmiennych objasniajacych
        for(int i=0; i<4; i++)
        {
            if(nrGrafu[i]==0)
            {
                nrGrafu[i] = max(nrGrafu, 4)+1;
            }
        }

        wybraneZmienne[0] = 0;
        wybraneZmienne[1] = 0;
        wybraneZmienne[2] = 0;
        wybraneZmienne[3] = 0;
        //wybranych = 1;
        wybranych = 0;
        int rozpatrywany, indexRozpatrywanego;
        for(int i=0; i<4; i++)
        {
            rozpatrywany = -1;
            for(int j=0;j<wybranych;j++)
            {
                if(nrGrafu[i]==nrGrafu[wybraneZmienne[j]])
                {
                    rozpatrywany = wybraneZmienne[j];
                    indexRozpatrywanego = j;
                    break;
                }
            }
            if(rozpatrywany==-1)
            {

                    wybranych++;
                    wybraneZmienne[wybranych-1] = i;

            }
            else
            {
                if(nrGrafu[i]==nrGrafu[rozpatrywany])
                {
                    if(liczbaSciezek[i]>=liczbaSciezek[rozpatrywany])
                    {
                        if(zaokr(fabs(R0[i][0]),"%.3f")==zaokr(fabs(R0[rozpatrywany][0]),"%.3f"))
                        {

                                wybranych++;
                                wybraneZmienne[wybranych-1] = i;

                        }
                        else if(zaokr(fabs(R0[i][0]),"%.3f")>zaokr(fabs(R0[rozpatrywany][0]),"%.3f"))
                        {

                                wybraneZmienne[indexRozpatrywanego] = i;

                        }
                    }
                }
            }
        }
        //wybranych--;
        if(wybranych>0)
        {
            mOutput->Lines->Add("");
            mOutput->Lines->Add("");
            mOutput->Lines->Add("Wybrane zmienne objasniajace:");
            for(int i=0; i<wybranych; i++)
            {
                mOutput->Lines->Add("");
                mOutput->SelText = "x";
                mOutput->SelText = wybraneZmienne[i]+1;        /// tu +1
            }
        }
        else
        {
           mOutput->Lines->Add("");
           mOutput->Lines->Add("");
           mOutput->Lines->Add("Nie wybrano ¿adnej zmiennej objasniajacej!");
        }
        ileZmiennych=4;
        bestKomb[0] = -1;// wybraneZmienne[0];
        bestKomb[1] = 2;
        bestKomb[2] = -1;
        bestKomb[3] = -1;
        bMNK->Enabled = true;
    }
    //Button3->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TfMain::bGrafyClick(TObject *Sender)
{
        okno->Caption = "Poziom istotnosci";
    okno->Position = poScreenCenter;
    okno->AutoSize = 1;

    panel->Parent = okno;
    panel->Alignment = taCenter;
    panel->Height = 70;
    TLabel *text = new TLabel(panel);
    text->Width = 50;
    text->Parent = panel;
    text->Caption = "Wybierz poziom istotnosci";
    text->Left = 30;
    text->Top = 5;

    wybor->Parent = panel;
    wybor->Caption = "0,1";
    wybor->Left = 40;
    wybor->Top = 20;

    wybor2->Parent = panel;
    wybor2->Caption = "0,05";
    wybor2->Left = 80;
    wybor2->Top = 20;
    TButton *przycisk = new TButton(panel);
    przycisk->Caption = "Ok";
    przycisk->Top = 40;
    przycisk->Left = 50;
    przycisk->Parent = panel;
    przycisk->OnClick = zatwierdz;
    okno->ShowModal();        
}
//---------------------------------------------------------------------------


void __fastcall TfMain::PaintBoxPaint(TObject *Sender)
{
 //rysowanie
    char buff[20];
    for(int i=0; i<cSciezki; i++)
    {
        PaintBox->Canvas->MoveTo(sciezki[i][0]+15,sciezki[i][1]+15);
        PaintBox->Canvas->LineTo(sciezki[i][2]+15,sciezki[i][3]+15);
    }
    char x[3];
    for(int i=0; i<cWierzcholki; i++)
    {
        strcpy(x, "x");
        PaintBox->Canvas->Ellipse(wierzcholki[i][0], wierzcholki[i][1], wierzcholki[i][2], wierzcholki[i][3]);
        sprintf(buff, "%d", i+1);
        strcat(x,buff);
        PaintBox->Canvas->TextOut(wierzcholki[i][0]+10, wierzcholki[i][1]+8, x);
    }        
}
//---------------------------------------------------------------------------



