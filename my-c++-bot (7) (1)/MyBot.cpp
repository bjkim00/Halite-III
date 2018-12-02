#include "hlt/game.hpp"
#include "hlt/constants.hpp"
#include "hlt/log.hpp"

#include <random>
#include <ctime>

using namespace std;
using namespace hlt;

int main(int argc, char* argv[]) {
    unsigned int rng_seed;
    if (argc > 1) {
        rng_seed = static_cast<unsigned int>(stoul(argv[1]));
    } else {
        rng_seed = static_cast<unsigned int>(time(nullptr));
    }
    mt19937 rng(rng_seed);

    Game game;
    // At this point "game" variable is populated with initial map data.
    // This is a good place to do computationally expensive start-up pre-processing.
    // As soon as you call "ready" function below, the 2 second per turn timer will start.
    game.ready("MyCppBot");
    bool hit = false;
    log::log("Successfully created bot! My Player ID is " + to_string(game.my_id) + ". Bot rng seed is " + to_string(rng_seed) + ".");
    vector<int> ship_status = {1, 1, 1, 1, 1};
    for (;;) {
        game.update_frame();
        shared_ptr<Player> me = game.me;
        unique_ptr<GameMap>& game_map = game.game_map;
        vector<Command> command_queue;
        for (const auto& ship_iterator : me->ships) {
            shared_ptr<Ship> ship = ship_iterator.second;
            log::log(to_string(hit));

            if(ship_status[ship->id] == 0 && hit == true)
            {
                if(ship->position == me->shipyard->position)
                {
                    log::log("HELL");
                    ship_status[ship->id] = 1;
                    hit = false;
                }
                else
                {
                    log::log("WHQT");
                    Direction move = game_map->naive_navigate(ship, me->shipyard->position);
                    command_queue.push_back(ship->move(move));
                    //break;
                }
            }
            else if(ship->halite >= 350)
            {
                log::log("Hello");
                ship_status[ship->id] = 0;
                log::log(to_string(ship_status[ship->id]));
                Direction move = game_map->naive_navigate(ship, me->shipyard->position);
                command_queue.push_back(ship->move(move));
                hit = true;
            }
            else
            {
                if (game_map->at(ship)->halite < constants::MAX_HALITE / 10 && ship->is_full() == false && ship_status[ship->id] == 1) {
                    log::log("WTF");
                    array<Position,4> what = {ship->position.get_surrounding_cardinals()};
                    for(int i = 0; i < 4; i++)
                    {
                        Position p = what[i];
                        if(game_map->at(p)->is_occupied() == true)
                        {
                            game_map->at(p)->mark_unsafe(ship);
                        }
                    }
                    Direction random_direction = game_map->naive_navigate(ship, me->shipyard->position);
                    command_queue.push_back(ship->move(random_direction));
                } else {
                    log::log("Nani");
                    command_queue.push_back(ship->stay_still());
                }
            }

        }
        
        array<Position,4> what2 = {me->shipyard->position.get_surrounding_cardinals()};
        bool dont = false;
        for(int i = 0; i < 4; i++)
        {
           Position p = what2[i];
           if(game_map->at(p)->is_occupied())
           {
                dont = true;
                break;
           }
        }
        
        if (game.turn_number <= 200 && me->halite >= constants::MAX_HALITE && !game_map->at(me->shipyard)->is_occupied() && dont == false)
        {
            command_queue.push_back(me->shipyard->spawn());
        }

        if (!game.end_turn(command_queue)) {
            break;
        }
    }

    return 0;
}
