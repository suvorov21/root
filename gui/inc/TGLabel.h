// @(#)root/gui:$Name:  $:$Id: TGLabel.h,v 1.9 2003/12/15 08:54:29 brun Exp $
// Author: Fons Rademakers   06/01/98

/*************************************************************************
 * Copyright (C) 1995-2000, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef ROOT_TGLabel
#define ROOT_TGLabel


//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TGLabel                                                              //
//                                                                      //
// This class handles GUI labels.                                       //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef ROOT_TGFrame
#include "TGFrame.h"
#endif
#ifndef ROOT_TGDimension
#include "TGDimension.h"
#endif
#ifndef ROOT_TGString
#include "TGString.h"
#endif

class TColor;

class TGLabel : public TGFrame {

protected:
   TGString      *fText;         // label text
   UInt_t         fTWidth;       // text width
   UInt_t         fTHeight;      // text height
   Int_t          fTMode;        // text drawing mode (ETextJustification)
   Bool_t         fTextChanged;  // has text changed
   GContext_t     fNormGC;       // graphics context used for drawing label
   FontStruct_t   fFontStruct;   // font to draw label
   Bool_t         fHasOwnFont;   // kTRUE - font defined locally,  kFALSE - globally

   virtual void DoRedraw();

   static const TGFont  *fgDefaultFont;
   static const TGGC    *fgDefaultGC;

public:
   static FontStruct_t  GetDefaultFontStruct();
   static const TGGC   &GetDefaultGC();

   TGLabel(const TGWindow *p, TGString *text,
           GContext_t norm = GetDefaultGC()(),
           FontStruct_t font = GetDefaultFontStruct(),
           UInt_t options = kChildFrame,
           Pixel_t back = GetDefaultFrameBackground());
   TGLabel(const TGWindow *p, const char *text,
           GContext_t norm = GetDefaultGC()(),
           FontStruct_t font = GetDefaultFontStruct(),
           UInt_t options = kChildFrame,
           Pixel_t back = GetDefaultFrameBackground());
   virtual ~TGLabel();

   virtual TGDimension GetDefaultSize() const { return TGDimension(fTWidth, fTHeight+1); }
   const TGString *GetText() const { return fText; }
   virtual void SetText(TGString *newText);
   void SetText(const char *newText) { SetText(new TGString(newText)); }
   void SetText(Int_t number) { SetText(new TGString(number)); }
   void SetTextJustify(Int_t tmode) { fTMode = tmode; }
   virtual void SetTextFont(TGFont *font, Bool_t global = kFALSE);
   virtual void SetTextFont(FontStruct_t font, Bool_t global = kFALSE);
   virtual void SetTextFont(const char *fontName, Bool_t global = kFALSE);
   virtual void SetTextColor(Pixel_t color, Bool_t global = kFALSE);
   virtual void SetTextColor(TColor *color, Bool_t global = kFALSE);
   Bool_t HasOwnFont() const;

   virtual void SavePrimitive(ofstream &out, Option_t *option);

   ClassDef(TGLabel,0)  // A label GUI element
};

#endif
