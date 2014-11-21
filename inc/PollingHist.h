#ifndef POLLINGHIST_H
#define POLLINGHIST_H


#include "a2display.h"
#include "TH2CB.h"
#include "TH2TAPS.h"

#include "TDirectory.h"
#include "TH1.h"
#include "TPaveLabel.h"
#include "TString.h"
#include "TList.h"

#include <string>
#include <iostream>

using namespace std;

template<class HistType>
class PollingHistogram: public HistType {
    ClassDef(PollingHistogram<HistType>,1);

protected:
    std::string pollname;
    bool message_displayed;
    TList* messages;

public:
    PollingHistogram( const std::string& toPoll, const std::string& name="", const std::string& title=""):
        HistType(name,title), pollname(toPoll), message_displayed(false)
    { messages = new TList();
        messages->IsOwner(); }

    virtual ~PollingHistogram() {}

    virtual void UpdateFromHist() {
        TH1* h(NULL);

        gDirectory->GetObject(pollname.c_str(), h);

        if( h ) {
            this->SetElements(*h);
            if( message_displayed ) {
                messages->Clear();
                message_displayed=false;
            }
        }
        else {
            if(!message_displayed) {
                TObject* list = this->GetListOfFunctions()->FindObject(messages);
                if(!list) {
                    this->GetListOfFunctions()->Add(messages);
                }

                TObject* message = MakeMessage(Form("Source histogram \"%s\" not found!", pollname.c_str()));
                messages->Add(message);
                message_displayed=true;
            }
        }
    }

    virtual TObject* MakeMessage( const std::string& m ) {

        const Double_t xmin = this->GetXaxis()->GetXmin();
        const Double_t xmax = this->GetXaxis()->GetXmax();
        const Double_t ymin = this->GetYaxis()->GetXmin();
        const Double_t ymax = this->GetYaxis()->GetXmax();

        const Double_t x1 = 0.2 * (xmax-xmin) + xmin;
        const Double_t x2 = 0.8 * (xmax-xmin) + xmin;
        const Double_t y1 = 0.2 * (ymax-ymin) + ymin;
        const Double_t y2 = 0.4 * (ymax-ymin) + ymin;

        TPaveLabel* message = new TPaveLabel(x1,y1,x2,y2,m.c_str());
        return message;
    }

    virtual void Draw(Option_t *option="") {
        UpdateFromHist();
        HistType::Draw(option);
    }

};

template class PollingHistogram<TH2CB>;
template class PollingHistogram<TH2TAPS>;

#endif
