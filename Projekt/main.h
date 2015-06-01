//---------------------------------------------------------------------------

#ifndef mainH
#define mainH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Dialogs.hpp>
 #include "matrix.h"
#include <ExtCtrls.hpp>
#include <jpeg.hpp>
//---------------------------------------------------------------------------
class TfMain : public TForm
{
__published:	// IDE-managed Components
        TMemo *mOutput;
        TOpenDialog *dZmienPlik;
        TButton *bZmien;
        TButton *bMacierzKorelacji;
        TButton *bMNK;
        TButton *bZakoncz;
        TButton *bGrafy;
        TPaintBox *PaintBox;
        void __fastcall bZmienClick(TObject *Sender);
        void __fastcall FormCreate(TObject *Sender);
        void __fastcall wypisz(AnsiString nazwa,matrix m);
        void __fastcall bMacierzKorelacjiClick(TObject *Sender);
        //void __fastcall bMetodaHellwigaClick(TObject *Sender);
        //void __fastcall wypiszKombinacjaIPojemnosci(int comb[], int k,int K);
        //void __fastcall kombinacjadlaMNK(int nr);
        //void __fastcall obliczPojemnoschij(int komb[],int l);
        void __fastcall bZakonczClick(TObject *Sender);
        void __fastcall bMNKClick(TObject *Sender);
        void __fastcall bWeryfikacjaClick(TObject *Sender);
        void __fastcall bGrafyClick(TObject *Sender);
        void __fastcall PaintBoxPaint(TObject *Sender);

private:	// User declarations
        void __fastcall zatwierdz(TObject *Sender);	// User declarations
   // void __fastcall dezaktywuj(TObject *Sender);	// User declarations
public:		// User declarations
        __fastcall TfMain(TComponent* Owner);
        
};
//---------------------------------------------------------------------------
extern PACKAGE TfMain *fMain;
//---------------------------------------------------------------------------
#endif
