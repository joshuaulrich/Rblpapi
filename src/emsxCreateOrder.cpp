// -*- mode: C++; c-indent-level: 4; c-basic-offset: 4; indent-tabs-mode: nil; -*-
//
//  emsxCreateOrder.cpp -- "Bloomberg EMSX create order" function for the BLP API
//
//  Copyright (C) 2013  Whit Armstrong
//  Copyright (C) 2015  Whit Armstrong and Dirk Eddelbuettel
//  Copyright (C) 2018  Whit Armstrong, Dirk Eddelbuettel, and Joshua Ulrich
//
//  This file is part of Rblpapi
//
//  Rblpapi is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 2 of the License, or
//  (at your option) any later version.
//
//  Rblpapi is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with Rblpapi.  If not, see <http://www.gnu.org/licenses/>.


#include <vector>
#include <string>
#include <blpapi_correlationid.h>
#include <blpapi_session.h>
#include <blpapi_service.h>
#include <blpapi_request.h>
#include <blpapi_event.h>
#include <blpapi_message.h>
#include <blpapi_element.h>
#include <Rcpp.h>
#include <blpapi_utils.h>

using BloombergLP::blpapi::Session;
using BloombergLP::blpapi::Service;
using BloombergLP::blpapi::Request;
using BloombergLP::blpapi::Event;
using BloombergLP::blpapi::Element;
using BloombergLP::blpapi::Message;
using BloombergLP::blpapi::MessageIterator;
using BloombergLP::blpapi::CorrelationId;

// This function is based on bdh.cpp and the EMSX API example here:
// https://github.com/tkim/emsx_api_repository/blob/master/EMSXFullSet_C%2B%2B/CreateOrder.cpp
//
// [[Rcpp::export]]
Rcpp::List createOrder_Impl(SEXP con_, std::string security_, SEXP size_,
    std::string side_, std::string type_, std::string tif_) {

    Session* session = reinterpret_cast<Session*>(
        checkExternalPointer(con_, "blpapi::Session*"));
/*
    // get the field info
    std::vector<FieldInfo> fldinfos(getFieldTypes(session, fields));
    std::vector<RblpapiT> rtypes;
    for(auto f : fldinfos) {
        rtypes.push_back(fieldInfoToRblpapiT(f.datatype,f.ftype));
    }

    // for bdh request all int fields as doubles b/c of implicit bbg conversion
    if (int_as_double) {
      std::transform(rtypes.begin(), rtypes.end(), rtypes.begin(),
                     [](RblpapiT x) { return x == RblpapiT::Integer || x == RblpapiT::Integer64 ? RblpapiT::Double : x; });
    }
*/
    const std::string srvname = "//blp/emapisvc_beta";
    Rcpp::Rcout << "Attempting to open service: " << srvname << std::endl;
    if (!session->openService(srvname.c_str())) {
        Rcpp::stop("Failed to open " + srvname);
    }

    Service service = session->getService(srvname.c_str());
    Request request = service.createRequest("CreateOrder");

    // The fields below are mandatory
    request.set("EMSX_TICKER", security_.c_str());
    request.set("EMSX_AMOUNT", 1000);
    request.set("EMSX_ORDER_TYPE", type_.c_str());
    request.set("EMSX_TIF", tif_.c_str());
    request.set("EMSX_HAND_INSTRUCTION", "ANY");
    request.set("EMSX_SIDE", side_.c_str());
    request.set("EMSX_LIMIT_PRICE", 123.45);

    // The fields below are optional
    //request.set("EMSX_ACCOUNT","TestAccount");
    //request.set("EMSX_BASKET_NAME", "HedgingBasket");
    //request.set("EMSX_BROKER", "BMTB");
    //request.set("EMSX_CFD_FLAG", "1");
    //request.set("EMSX_CLEARING_ACCOUNT", "ClrAccName");
    //request.set("EMSX_CLEARING_FIRM", "FirmName");
    //request.set("EMSX_CUSTOM_NOTE1", "Note1");
    //request.set("EMSX_CUSTOM_NOTE2", "Note2");
    //request.set("EMSX_CUSTOM_NOTE3", "Note3");
    //request.set("EMSX_CUSTOM_NOTE4", "Note4");
    //request.set("EMSX_CUSTOM_NOTE5", "Note5");
    //request.set("EMSX_EXCHANGE_DESTINATION", "ExchDest");
    //request.set("EMSX_EXEC_INSTRUCTION", "Drop down values from EMSX Ticket");
    //request.set("EMSX_GET_WARNINGS", "0");
    //request.set("EMSX_GTD_DATE", "20170105");
    //request.set("EMSX_INVESTOR_ID", "InvID");
    //request.set("EMSX_LIMIT_PRICE", 123.45);
    //request.set("EMSX_LOCATE_BROKER", "BMTB");
    //request.set("EMSX_LOCATE_ID", "SomeID");
    //request.set("EMSX_LOCATE_REQ", "Y");
    //request.set("EMSX_NOTES", "Some notes");
    //request.set("EMSX_ODD_LOT", "0");
    //request.set("EMSX_ORDER_ORIGIN", "");
    //request.set("EMSX_ORDER_REF_ID", "UniqueID");
    //request.set("EMSX_P_A", "P");
    //request.set("EMSX_RELEASE_TIME", 34341);
    //request.set("EMSX_REQUEST_SEQ", 1001);
    //request.set("EMSX_SETTLE_CURRENCY", "USD");
    //request.set("EMSX_SETTLE_DATE", 20170106);
    //request.set("EMSX_SETTLE_TYPE", "T+2");
    //request.set("EMSX_STOP_PRICE", 123.5);

    CorrelationId requestID = CorrelationId();

    Rcpp::Rcout << "Attempting to send request: " << request << std::endl;
    session->sendRequest(request, requestID);
    //createStandardRequest(request, securities, fields, options_, overrides_);

    while (true) {
        Event event = session->nextEvent();
        Rcpp::Rcout << "Received event type: " << event.eventType() << std::endl;
        switch (event.eventType()) {
          //case Event::RESPONSE:
          //case Event::PARTIAL_RESPONSE:
              break;
          default:
              MessageIterator msgIter(event);
              while (msgIter.next()) {
                  Message msg = msgIter.message();
                  Rcpp::Rcout << "Received message: " << msg << std::endl;
                  // FIXME:: capture msg here for logging
              }
              break;
        }
        if (event.eventType() == Event::RESPONSE) { break; }
    }
    Rcpp::List ans(1);
    return ans;
}
