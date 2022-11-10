# AirlineTycoon

This repository aims to complete the partial source code that is provided as a free bonus in the GOG release
of Airline Tycoon Deluxe.

To run it you'll need the game assets from the either the First Class, Evolution or
Deluxe edition of the game. You can purchase these assets from GOG.com: https://www.gog.com/game/airline_tycoon_deluxe

## Major Additions
- Native Linux support
- Dedicated server browser and NAT-punchthrough multiplayer (open source server at: https://github.com/WizzardMaker/ATDMasterServer)

## License

The code in the repository is licensed under the terms included in the GOG release. As such the code can
only be used for non-commercial purposes and remains property of BFG.

It is therefore *not* open-source in the free software sense, for more information refer to the License.txt.

## Building

Before building remember to clone the submodules:

```
git submodule update --init
```

### Windows
The project can be build with Visual Studio.

#### Dependencies
These dependencies need to be downloaded separately:
```ps
cd cmake
# SDL2, SDL2-Mixer, SDL2-TTF, SDL2-Image
wget "https://github.com/libsdl-org/SDL_ttf/releases/download/release-2.20.1/SDL2_ttf-devel-2.20.1-VC.zip" -outfile "sdl2-ttf.zip"
wget "https://github.com/libsdl-org/SDL_mixer/releases/download/release-2.6.2/SDL2_mixer-devel-2.6.2-VC.zip" -outfile "sdl2-mixer.zip"
wget "https://github.com/libsdl-org/SDL_image/releases/download/release-2.6.2/SDL2_image-devel-2.6.2-VC.zip" -outfile "sdl2-image.zip"
wget "https://github.com/libsdl-org/SDL/releases/download/release-2.24.2/SDL2-devel-2.24.2-VC.zip" -outfile "sdl2.zip"
```
Extract the content of those zips to a folder structure like this:
```
cmake/
    sdl2/
        include/*
        lib/*
        ...
    sdl2-image/
        include/*
        lib/*
        ...
    ...
```

The visual studio project files also need to be created for the enet dependency. Use the "Developer Command Prompt for VS 2022" or "Developer Powershell for VS 2022" and create the project files like this:
```ps
# Go to root of repo...
cd libs/enet
cmake .
```

You have to change the build output path of enet to point to `$(SolutionDir)BUILD\enet\$(Configuration)\enet.lib`

#### Building
To build the application, open the project solution with Visual Studio, select your configuration and then build like any other VS project.

### Linux
The project hast to be build with cmake/make

#### Dependencies
Download the SDL dependencies like this:
```sh
apt-get install libsdl2-dev
apt-get install libsdl2-image-dev
apt-get install libsdl2-mixer-dev
apt-get install libsdl2-ttf-dev
```

Make sure that SDL2 is at least version 2.10

#### Building (WIP)
1. Prepare the cmake environment `cmake .`
2. Build the project with `make`

----

## Changes

General:
* Now runnable on Linux

Statistics screen:
* Showing far more categories where money was spent
* Information visible depends on skill of financial advisor (own data) or skill of spy (competitors)
* Unlimited statistics: Store statistics data for each day without limit
* Fix rendering of graph when zooming out

Information menu:
* Much more information on balance sheet
* New financial summary
* Showing multiple balance sheets for day / week / overall
* Showing business operations and operating balance
* More information from spy (e.g. weekly balance and financial summary for each competitor)
* Showing information from kerosene advisor (quality/value of kerosene, money saved through tanks)

Keyboard navigation:
* Allow Enter/Backspace in calculator
* Enable keyboard navigation in Laptop / Globe (arrow keys)
* Enable keyboard navigation in HR folder
    * Flip using arrow keys
    * Jump 10/100 pages in HR files using Shift/Ctrl
    * Change salary using +/-
    * Hire/fire using Enter/Backspace
* Enable keyboard navigation in plane prop menu (arrow keys, jump using Shift/Ctrl)
* Arrow key navigation for many different menus

Employees:
* Fix problem where no competent personal can be found in long running games
* More pilots/attendants available for hire
* Slightly increase competence of randomly generated employees
* Generate randomized advisors as well
* Replace employees if not hired within 7 days
* Sort list by skill
* Update worker happiness based on salary
    * Chance to increase/decrease happiness each day based on how much salary is higher/lower than original salary
* The 10% change when increasing/decreasing salary now always refers to 10% of original salary

Kerosene:
* Impact of bad kerosene depends on ratio of bad kerosene in tank
* Do not remember selected kerosene quality for auto purchase
* Adjust impact of bad kerosene: Quadratic function now
* Offer much larger kerosene tanks
* Kerosene advisor gives hints to save money

Bug fixes:
* Frozen windows on laptop fixed
* Integer overflow fixed when emitting lots of stock (resulted in loosing money when emitting)
* Fixed formula for credit limit
* Stock trading: Show correct new account balance in form (including fee)
* Fix saving/reloading of plane equipment configuration
* Fix bug in gate planning ('no gate' warning depite plenty of free gates available)
* Fix distant rendering of sticky notes in the boss office
* Use correct security measure to protect against route stealing
* Fix calculation of passenger happiness
    * Set passenger rating based on quality + small randomized delta (old code could yield just 'okay' even with high quality)
    * Passengers will tolerate high prices if quality is good
* Fix sabotage that puts leaflets into opponent's plane
    * Now passenger happiness is booked to the statistic of the sabotaging player
* 'Plane crash' sabotage now also affects stock price of victim
* Fix calculation of plane repair cost
    * All cost will show up in plane saldo
    * All cost will show up in plane repair cost total
    * Always rotate list of plane saldo for past 7 days
* Consider also number of first class passengers for statistics
* Do not show route utilization by defeated players

AI:
* Uses now same credit limit
* Uses now same rules for trading stock
    * Trading fee (100 + 10% of volume) now also for NPCs (fee existed only for player)
    * Do not trade in steps of 1000 (worsened price for player only!)
    * Align function to re-calculate stock price after trade
* Uses now same rules for emitting stock
* Remove sabotage advantages
    * Computer now has to pay for sabotage as well
    * Consider all security measures (e.g. plane crash not possible anymore if plane is protected)
    * Align calculation of arab trust for player and computer
* Remove strange reduction of flight cost in calculation of image change (was a disadvantage for computer player)
* Computer player pays real cost for plane upgrades
* Reduce (~ half) cost of plane security upgrades
* Fix random crash (during checking of flight plans)

Misc:
* Spy reports enemy activity based on skill
* ArabAir opens one hour earlier
* Calculate route utilization as average of previous 7 days
* Adding NOTFAIR cheat to make competitors much richer
* Adding ODDONEOUT cheat to improve image of competitors
* Use player colors when showing routes on laptop
* Buy kerosene by clicking price chart
* Change tooltip of savegames (number of days played)
* Decryption of data files with the run argument "/savedata"
