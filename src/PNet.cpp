//============================================================================================
// PNet.cpp : Routinen zum verwalten der Spieler im Netzwerk
//============================================================================================
#include "StdAfx.h"
#include "AtNet.h"

extern bool bgIsLoadingSavegame;

//--------------------------------------------------------------------------------------------
// Returns the number of players on which this computer will send information:
//--------------------------------------------------------------------------------------------
SLONG PLAYER::NetSynchronizeGetNum() {
    if (Sim.bIsHost != 0) {
        SLONG c = 0;
        SLONG n = 0;

        n = 1;
        for (c = 0; c < 4; c++) {
            if (Sim.Players.Players[c].Owner == 1 && (Sim.Players.Players[c].IsOut == 0)) {
                n++;
            }
        }

        return (n);
    }
    return (1);
}

//--------------------------------------------------------------------------------------------
// Sends the data concerning image to other players:
//--------------------------------------------------------------------------------------------
void PLAYER::NetSynchronizeImage() {
    TEAKFILE Message;
    SLONG c = 0;

    Message.Announce(1024);

    Message << ATNET_SYNC_IMAGE << NetSynchronizeGetNum();

    // Für den lokalen Spieler und (wenn dies der Server ist) auch für Computerspieler:
    for (c = 0; c < 4; c++) {
        PLAYER &qPlayer = Sim.Players.Players[c];

        if (qPlayer.Owner == 0 || ((Sim.bIsHost != 0) && qPlayer.Owner == 1 && (qPlayer.IsOut == 0))) {
            SLONG d = 0;

            Message << c << qPlayer.Image << qPlayer.ImageGotWorse;

            for (d = 0; d < 4; d++) {
                Message << qPlayer.Sympathie[d];
            }

            for (d = Routen.AnzEntries() - 1; d >= 0; d--) {
                Message << qPlayer.RentRouten.RentRouten[d].Image;
            }
            for (d = Cities.AnzEntries() - 1; d >= 0; d--) {
                Message << qPlayer.RentCities.RentCities[d].Image;
            }
        }
    }

    SIM::SendMemFile(Message);
}

//--------------------------------------------------------------------------------------------
// Sends the data concerning money to other players:
//--------------------------------------------------------------------------------------------
void PLAYER::NetSynchronizeMoney() {
    TEAKFILE Message;
    SLONG c = 0;

    Message.Announce(256);

    Message << ATNET_SYNC_MONEY << NetSynchronizeGetNum();

    // Für den lokalen Spieler und (wenn dies der Server ist) auch für Computerspieler:
    for (c = 0; c < 4; c++) {
        PLAYER &qPlayer = Sim.Players.Players[c];

        if (qPlayer.Owner == 0 || ((Sim.bIsHost != 0) && qPlayer.Owner == 1 && (qPlayer.IsOut == 0))) {
            SLONG d = 0;

            Message << c;

            Message << qPlayer.Money << qPlayer.Credit << qPlayer.Bonus << qPlayer.AnzAktien << qPlayer.MaxAktien << qPlayer.TrustedDividende
                    << qPlayer.Dividende;

            for (d = 0; d < 4; d++) {
                Message << qPlayer.OwnsAktien[d] << qPlayer.AktienWert[d];
            }
            for (d = 0; d < 10; d++) {
                Message << qPlayer.Kurse[d];
            }
        }
    }

    SIM::SendMemFile(Message);
}

//--------------------------------------------------------------------------------------------
// Sends the data concerning routes to other players:
//--------------------------------------------------------------------------------------------
void PLAYER::NetSynchronizeRoutes() {
    TEAKFILE Message;
    SLONG c = 0;

    Message.Announce(1024);

    Message << ATNET_SYNC_ROUTES << NetSynchronizeGetNum();

    // Für den lokalen Spieler und (wenn dies der Server ist) auch für Computerspieler:
    for (c = 0; c < 4; c++) {
        PLAYER &qPlayer = Sim.Players.Players[c];

        if (qPlayer.Owner == 0 || ((Sim.bIsHost != 0) && qPlayer.Owner == 1 && (qPlayer.IsOut == 0))) {
            Message << c;

            for (SLONG d = Routen.AnzEntries() - 1; d >= 0; d--) {
                Message << qPlayer.RentRouten.RentRouten[d].Rang << qPlayer.RentRouten.RentRouten[d].LastFlown << qPlayer.RentRouten.RentRouten[d].Image
                        << qPlayer.RentRouten.RentRouten[d].Miete << qPlayer.RentRouten.RentRouten[d].Ticketpreis
                        << qPlayer.RentRouten.RentRouten[d].TicketpreisFC << qPlayer.RentRouten.RentRouten[d].TageMitVerlust
                        << qPlayer.RentRouten.RentRouten[d].TageMitGering;
            }
        }
    }

    SIM::SendMemFile(Message);
}

//--------------------------------------------------------------------------------------------
// Läßt einen Spieler die Ticketpreis verändern:
//--------------------------------------------------------------------------------------------
void PLAYER::NetRouteUpdateTicketpreise(SLONG RouteId, SLONG Ticketpreis, SLONG TicketpreisFC) const {
    SIM::SendSimpleMessage(ATNET_SYNCROUTECHANGE, 0, PlayerNum, RouteId, Ticketpreis, TicketpreisFC);
}

//--------------------------------------------------------------------------------------------
// Sends the data concerning flags to other players:
//--------------------------------------------------------------------------------------------
void PLAYER::NetSynchronizeFlags() {
    TEAKFILE Message;
    SLONG c = 0;

    Message.Announce(64);

    Message << ATNET_SYNC_FLAGS << NetSynchronizeGetNum();

    // Für den lokalen Spieler und (wenn dies der Server ist) auch für Computerspieler:
    for (c = 0; c < 4; c++) {
        PLAYER &qPlayer = Sim.Players.Players[c];

        if (qPlayer.Owner == 0 || ((Sim.bIsHost != 0) && qPlayer.Owner == 1 && (qPlayer.IsOut == 0))) {
            Message << c;

            Message << qPlayer.SickTokay << qPlayer.RunningToToilet << qPlayer.PlayerSmoking << qPlayer.Stunned << qPlayer.OfficeState << qPlayer.Koffein
                    << qPlayer.NumFlights << qPlayer.WalkSpeed << qPlayer.WerbeBroschuere << qPlayer.TelephoneDown << qPlayer.Presseerklaerung
                    << qPlayer.SecurityFlags << qPlayer.PlayerStinking << qPlayer.RocketFlags << qPlayer.LastRocketFlags;
        }
    }

    SIM::SendMemFile(Message);
}

//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
void PLAYER::NetSynchronizeItems() {
    TEAKFILE Message;
    SLONG c = 0;

    Message.Announce(64);

    Message << ATNET_SYNC_ITEMS << NetSynchronizeGetNum();

    // Für den lokalen Spieler und (wenn dies der Server ist) auch für Computerspieler:
    for (c = 0; c < 4; c++) {
        PLAYER &qPlayer = Sim.Players.Players[c];

        if (qPlayer.Owner == 0 || ((Sim.bIsHost != 0) && qPlayer.Owner == 1 && (qPlayer.IsOut == 0))) {
            Message << c;

            for (SLONG d = 0; d < 6; d++) {
                Message << qPlayer.Items[d];
            }
        }
    }

    SIM::SendMemFile(Message);
}

//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
void PLAYER::NetSynchronizePlanes() {
    if (Sim.bIsHost != 0) {
        TEAKFILE Message;
        SLONG c = 0;

        Message.Announce(1024);

        SLONG count = 0;
        for (c = 0; c < 4; c++) {
            PLAYER &qPlayer = Sim.Players.Players[c];

            if (qPlayer.Owner == 1 && (qPlayer.IsOut == 0)) {
                count++;
            }
        }

        Message << ATNET_SYNC_PLANES << count;

        // Wwenn dies der Server ist für alle Computerspieler:
        for (c = 0; c < 4; c++) {
            PLAYER &qPlayer = Sim.Players.Players[c];

            if (qPlayer.Owner == 1 && (qPlayer.IsOut == 0)) {
                Message << c;
                Message << qPlayer.Planes << qPlayer.Auftraege << qPlayer.Frachten << qPlayer.RentCities;
            }
        }

        SIM::SendMemFile(Message);
    }
}

//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
void PLAYER::NetSynchronizeMeeting() {
    TEAKFILE Message;
    SLONG c = 0;

    Message.Announce(64);

    SLONG count = 0;
    for (c = 0; c < 4; c++) {
        PLAYER &qPlayer = Sim.Players.Players[c];

        if ((((Sim.bIsHost != 0) && qPlayer.Owner == 1) || qPlayer.Owner == 0) && (qPlayer.IsOut == 0)) {
            count++;
        }
    }

    Message << ATNET_SYNC_MEETING << count;

    // Wenn dies der Server ist für alle Computerspieler:
    for (c = 0; c < 4; c++) {
        PLAYER &qPlayer = Sim.Players.Players[c];

        if ((((Sim.bIsHost != 0) && qPlayer.Owner == 1) || qPlayer.Owner == 0) && (qPlayer.IsOut == 0)) {
            Message << c;
            Message << qPlayer.ArabTrust << qPlayer.ArabMode << qPlayer.ArabMode2 << qPlayer.ArabMode3 << qPlayer.ArabActive;
            Message << qPlayer.ArabOpfer << qPlayer.ArabOpfer2 << qPlayer.ArabOpfer3 << qPlayer.ArabPlane << qPlayer.ArabHints;
            Message << qPlayer.NumPassengers << qPlayer.NumFracht;
        }
    }

    Message << Sim.bIsHost;
    if (Sim.bIsHost != 0) {
        Message << Sim.SabotageActs;
    }

    SIM::SendMemFile(Message);
}

//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
void PLAYER::NetSynchronizeSabotage() const {
    TEAKFILE Message;

    Message.Announce(128);

    Message << ATNET_SABOTAGE_ARAB << PlayerNum;

    Message << ArabOpfer << ArabMode << ArabActive << ArabPlane << ArabOpfer2 << ArabMode2 << ArabOpfer3 << ArabMode3;

    SIM::SendMemFile(Message);
}

//--------------------------------------------------------------------------------------------
// Transfers a flightplan
//--------------------------------------------------------------------------------------------
void PLAYER::NetUpdateFlightplan(SLONG PlaneId) {
    TEAKFILE Message;

    Message.Announce(1024);

    Message << ATNET_FP_UPDATE;
    Message << PlaneId << PlayerNum;
    Message << Planes[PlaneId].Flugplan;

    SIM::SendMemFile(Message);
}

//--------------------------------------------------------------------------------------------
// Player took an order flight ==> Tell the others:
// Type: 1 - LastMinute
// Type: 2 - Reisebüro
// Type: 3 - Fracht
// Type: 4 - Ausland, City = CityIndex
// This function only handle the blackboard in the room; It does not update the player's data
//--------------------------------------------------------------------------------------------
void PLAYER::NetUpdateTook(SLONG Type, SLONG Index, SLONG City) const {
    TEAKFILE Message;

    Message.Announce(128);

    Message << ATNET_PLAYER_TOOK << PlayerNum << Type << Index << City;

    SIM::SendMemFile(Message);
}

//--------------------------------------------------------------------------------------------
// Let's the player take the order on other computers, too:
//--------------------------------------------------------------------------------------------
void PLAYER::NetUpdateOrder(const CAuftrag &auftrag) const {
    TEAKFILE Message;

    Message.Announce(128);

    Message << ATNET_TAKE_ORDER;
    Message << PlayerNum << auftrag;

    SIM::SendMemFile(Message);
}

//--------------------------------------------------------------------------------------------
// Let's the player take the order on other computers, too:
//--------------------------------------------------------------------------------------------
void PLAYER::NetUpdateFreightOrder(const CFracht &auftrag) const {
    TEAKFILE Message;

    Message.Announce(128);

    Message << ATNET_TAKE_FREIGHT;
    Message << PlayerNum << auftrag;

    SIM::SendMemFile(Message);
}

//--------------------------------------------------------------------------------------------
// Updates the rental data of one route and back:
//--------------------------------------------------------------------------------------------
void PLAYER::NetUpdateRentRoute(SLONG Route1Id, SLONG Route2Id) {
    TEAKFILE Message;

    Message.Announce(128);

    Message << ATNET_TAKE_ROUTE;

    Message << PlayerNum << Route1Id << Route2Id;
    Message << RentRouten.RentRouten[Route1Id];
    Message << RentRouten.RentRouten[Route2Id];

    SIM::SendMemFile(Message);
}

//--------------------------------------------------------------------------------------------
// Updates the Kooperation status:
//--------------------------------------------------------------------------------------------
void PLAYER::NetSynchronizeKooperation() const {
    TEAKFILE Message;

    Message.Announce(128);

    Message << ATNET_DIALOG_KOOP;

    Message << PlayerNum << Kooperation;

    SIM::SendMemFile(Message);
}

//--------------------------------------------------------------------------------------------
// Updates the total number of workers and which planes they work on:
//--------------------------------------------------------------------------------------------
void PLAYER::NetUpdateWorkers() {
    TEAKFILE Message;
    SLONG m = 0;
    SLONG n = 0;
    SLONG c = 0;

    if (bgIsLoadingSavegame) {
        return;
    }

    Message.Announce(128);

    UpdateStatistics();

    Message << ATNET_PERSONNEL;

    m = static_cast<SLONG>(Statistiken[STAT_ZUFR_PERSONAL].GetAtPastDay(0));
    n = static_cast<SLONG>(Statistiken[STAT_MITARBEITER].GetAtPastDay(0));

    Message << PlayerNum << m << n;

    for (c = 0; c < Planes.AnzEntries(); c++) {
        if (Planes.IsInAlbum(c) != 0) {
            Message << c;
            Message << Planes[c].AnzPiloten;
            Message << Planes[c].AnzBegleiter;
            Message << Planes[c].PersonalQuality;
        }
    }

    c = -1;
    Message << c;

    SIM::SendMemFile(Message);

    if ((Sim.bIsHost != 0 && Owner == 1 && !RobotUse(ROBOT_USE_FAKE_PERSONAL)) || Owner == 0) {
        SLONG c = 0;
        SLONG d = 0;

        for (c = d = 0; c < Workers.Workers.AnzEntries(); c++) {
            if (Workers.Workers[c].Employer == PlayerNum) {
                d += Workers.Workers[c].Gehalt;
            }
        }
        Statistiken[STAT_GEHALT].SetAtPastDay(-d);

        SIM::SendSimpleMessage(ATNET_SYNCGEHALT, 0, Sim.localPlayer, d);
    }
}

//--------------------------------------------------------------------------------------------
// Organizes saving in the network:
//--------------------------------------------------------------------------------------------
void PLAYER::NetSave(DWORD UniqueGameId, SLONG CursorY, const CString &Name) {
    TEAKFILE Message;

    Message.Announce(128);

    Message << ATNET_IO_SAVE;

    Message << UniqueGameId << CursorY << Name;

    SIM::SendMemFile(Message);
}

//--------------------------------------------------------------------------------------------
// Broadcasts a plane's properties:
//--------------------------------------------------------------------------------------------
void PLAYER::NetUpdatePlaneProps(SLONG PlaneId) {
    TEAKFILE Message;

    Message.Announce(128);

    if (bgIsLoadingSavegame) {
        return;
    }

    Message << ATNET_PLANEPROPS;

    Message << PlayerNum << PlaneId;
    Message << MechMode;

    if (PlaneId != -1) {
        CPlane &qPlane = Planes[PlaneId];

        Message << qPlane.Sitze << qPlane.SitzeTarget << qPlane.Essen << qPlane.EssenTarget << qPlane.Tabletts << qPlane.TablettsTarget << qPlane.Deco
                << qPlane.DecoTarget << qPlane.Triebwerk << qPlane.TriebwerkTarget << qPlane.Reifen << qPlane.ReifenTarget << qPlane.Elektronik
                << qPlane.ElektronikTarget << qPlane.Sicherheit << qPlane.SicherheitTarget;

        Message << qPlane.WorstZustand << qPlane.Zustand << qPlane.TargetZustand;
        Message << qPlane.AnzBegleiter << qPlane.MaxBegleiter;
    }

    SIM::SendMemFile(Message);
}

//--------------------------------------------------------------------------------------------
// Broadcasts a players kerosine state:
//--------------------------------------------------------------------------------------------
void PLAYER::NetUpdateKerosin() const {
    TEAKFILE Message;

    Message.Announce(128);

    Message << ATNET_SYNCKEROSIN;
    Message << PlayerNum << Tank << TankOpen << TankInhalt << KerosinQuali << KerosinKind << TankPreis;

    SIM::SendMemFile(Message);
}

//--------------------------------------------------------------------------------------------
// Broadcasts a xplane buy:
//--------------------------------------------------------------------------------------------
void PLAYER::NetBuyXPlane(SLONG Anzahl, CXPlane &plane) const {
    TEAKFILE Message;

    Message.Announce(128);

    Message << ATNET_BUY_NEWX;
    Message << PlayerNum << Anzahl << plane;

    SIM::SendMemFile(Message);
}
