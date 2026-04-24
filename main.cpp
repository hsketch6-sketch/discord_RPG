
#include <string>
#include <dpp/dpp.h> // dpp 라이브러리를 쓰게해준다
#include <map> // std::map<dpp::snowflake, Player> player_db;를 쓰기위해 필요한 include이다.
#include <iostream>
#include <fstream>//저장에 필요한
#include <sstream> // indlude들 이다.
#include <dpp/nlohmann/json.hpp> //저장에 필요한 JOSN파일이다.(txt도 가능은 하지만, 유지보수가 힘들다)
#include <mutex> //레이스 컨디션 방지를 쓸려면 필요한것
#include <random>  //랜덤을 돌릴때 필요한 include이다.
//  std::lock_guard<std::mutex> lock(db_mutex); 는 레이스 컨디션을 막기 위한 장치이다.

using json = nlohmann::json; // json을 편하게 쓸려는 시도
struct Player {
	dpp::snowflake user_id; // 유저의 고유 ID로, 보통 Id는 너무커 이런 전용 타입을 사용한다.
	int hp = 100;
	int level = 1;
	int max_hp = 100 + (level * 5);
	long long gold = 0;
	int str = 10 + (level * 5);
	int dice = 0;
	int maxexp = 100 + (level * 20);
	int exp = 0;
	time_t last_hunt = 0;
	bool weapon = false;
	int floor = 1;
};

std::map<dpp::snowflake, Player> Player_db; //유저의 고유 Id를 찾아서 Player 구조체의 정보가 나오게 한다.
std::mutex db_mutex;  //혹시나 레이스 컨디션의 문제를 방지하기 위해서
//불러오는 함수
void load_data() {
    try {
        std::ifstream file("players.json");
        if (!file.is_open())  {
						std::cout << "[처음 시작한 플레이어 입니다]" << std::endl;
			file.close();
			return;
		}
        file.seekg(0, std::ios::end);
        if (file.tellg() <= 0) { 
            file.close();
            return; 
        }
        file.seekg(0, std::ios::beg);

        json j_all;
        try {
            file >> j_all;
        } catch (const json::parse_error& e) {
            std::cerr << "[시스템] JSON 깨짐 감지: " << e.what() << std::endl;
            file.close();
            return;
        }
        file.close();

        if (j_all.is_array()) {
            std::lock_guard<std::mutex> lock(db_mutex); // 데이터 넣을 때 잠금
            for (auto& j_user : j_all) {
                if (!j_user.contains("id")) continue;
                
                dpp::snowflake id = std::stoull(j_user["id"].get<std::string>());
                Player p;
                p.user_id = id;
                p.hp = j_user.value("hp", 100);
                p.level = j_user.value("level", 1);
                p.exp = j_user.value("exp", 0);
                p.maxexp = j_user.value("maxexp", 120);
                p.max_hp = j_user.value("max_hp", 105);
                p.gold = j_user.value("gold", 0LL);
                p.str = j_user.value("str", 15);
                p.dice = j_user.value("dice", 0);
                
                Player_db[id] = p; // 메모리에 로드
            }
            std::cout << "[시스템] " << Player_db.size() << "명의 유저 데이터를 로드했습니다." << std::endl;
        }
    } catch (...) {
        std::cerr << "[시스템] 알 수 없는 로드 에러 발생" << std::endl;
    }
}

//저장함수
	void save_data() {

		json j_all = json::array();
		{
			std::lock_guard<std::mutex> lock(db_mutex); // 정보 읽을때만 잠깐 잠궈라
			for (auto const& [id, p] : Player_db) {
				json j;
				j["id"] = std::to_string(p.user_id);
				j["hp"] = p.hp;
				j["level"] = p.level;
				j["exp"] = p.exp;
				j["maxexp"] = p.maxexp;
				j["max_hp"] = p.max_hp;
				j["gold"] = p.gold;
				j["str"] = p.str;
				j["dice"] = p.dice;
				j_all.push_back(j);
			}
		}
			std::ofstream file("players.json"); // json을 열고
			file << j_all.dump(4); // 보기 좋게 바꾼뒤
			file.close(); // 닫아라
		}
	
//레벨업 함수
void level_up_check(Player& P, const dpp::slashcommand_t& event) {

	while (P.exp >= P.maxexp) {
		P.exp -= P.maxexp; // 현재 경험치에서 소모
		P.level++;         // 레벨 상승
		P.max_hp = 100 + (P.level * 5);
		P.str = 10 + (P.level * 5);
		P.maxexp = 100 + (P.level * 20);
		P.hp = P.max_hp;
	}
}
//정보 함수
void handle_info(const dpp::slashcommand_t& event, Player&P) {

	dpp::embed embed = dpp::embed() // 이쁜 UI
		.set_color(dpp::colors::cyan)           //제목(강조되게)
		.set_title("📊 플레이어 정보")           //그외들
		.add_field("❤️ HP", std::to_string(P.hp) + " / " + std::to_string(P.max_hp), true) // 하늘색 테두리
		.add_field("⚔️ 공격력", std::to_string(P.str), false) // true=줄 안바꾸기 false =줄바꾸기
		.add_field("💰 보유 골드", std::to_string(P.gold) + " G", false)
	.add_field("⭐ 현재 경험치", std::to_string(P.exp) + " / " + std::to_string(P.maxexp), false);


	// 2. 메시지를 보낸다(정보)
	event.reply(dpp::message().add_embed(embed).set_flags(dpp::m_ephemeral));
}

//사냥 함수		
void handle_hunt(const dpp::slashcommand_t& event, Player& P) {
	int rool = 10;
	{
		std::lock_guard<std::mutex> lock(db_mutex);
		time_t now = time(NULL); //쿨타임
		if (now - P.last_hunt < 60) {
			event.reply(dpp::message("아직 쿨타임 입니다!").set_flags(dpp::m_ephemeral));
			return;
		}
		P.last_hunt = now;
		P.gold += rool * 10;
		P.exp += 20; 
		level_up_check(P, event); 
	}
	event.reply(dpp::message(" 사냥 성공! " + std::to_string(rool * 10) + "G와 20경험치를 획득했습니다.").set_flags(dpp::m_ephemeral));
	save_data();
}
//도박 함수
void justdice(const dpp::slashcommand_t& event, Player& P) {
	static std::random_device rd;
	static std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(0, 1);
	int dice = dis(gen);
	{
		std::lock_guard<std::mutex> lock(db_mutex);
		if (P.gold < 100) {
			event.reply(dpp::message("돈이 부족합니다!"));
			return;
		}
		else if (dice == 0) {
			P.gold = P.gold - 100;
			event.reply(dpp::message("실패 입니다.. 내 100G가..현재 골드:" + std::to_string(P.gold) + "G)").set_flags(dpp::m_ephemeral));
			save_data();
		}
		else if (dice == 1) {
			P.gold = P.gold + 100;
			event.reply(dpp::message("성공입니다! 100G를 얻었습니다!현재 골드:" + std::to_string(P.gold) + "G"));
			save_data();
		}
	}
};
		

int main() {
	srand(static_cast<unsigned int>(time(NULL)));

dpp::cluster bot(std::getenv("BOT_TOKEN")); 


	// 3. 로그 활성화 (에러 확인용)
	bot.on_log(dpp::utility::cout_logger());

	// 4. 데이터 로딩 (안전하게 한 번만 호출)
	std::cout << "[시스템] 데이터 로딩 중..." << std::endl;
	load_data();

	// 5. 봇 준비 이벤트 (명령어 등록)
	bot.on_ready([&bot](const dpp::ready_t& event) {
		std::cout << "[시스템] 연결 성공! 봇이 온라인입니다." << std::endl;
		if (dpp::run_once<struct register_bot_commands>()) {
			bot.global_command_create(dpp::slashcommand("정보", "내 캐릭터의 정보를 확인합니다.", bot.me.id));
			bot.global_command_create(dpp::slashcommand("사냥", "간단한 사냥을 하고 옵니다.", bot.me.id));
			bot.global_command_create(dpp::slashcommand("도박", "실제로는 이런거 하지 맙시다.(판돈=100G)", bot.me.id));
		}
		});

	// 6. 슬래시 커맨드 처리
	bot.on_slashcommand([&](const dpp::slashcommand_t& event) {
		dpp::snowflake caller_id = event.command.usr.id;
		dpp::snowflake allowed_channel_id = 1367290445646008442;
		if (event.command.channel_id != allowed_channel_id) {
			event.reply(dpp::message("이곳에서는 동작하지 않습니다!").set_flags(dpp::m_ephemeral));
				return;
		}
		bool new_Player = false;
		{
			std::lock_guard<std::mutex> lock(db_mutex);
		// 신규 유저 등록
			if (Player_db.find(caller_id) == Player_db.end()) {
				Player_db[caller_id].user_id = caller_id;
				new_Player = true;
			}
		}
		if (new_Player == true) {
			save_data();
		}
		if (event.command.get_command_name() == "정보") {
			handle_info(event, Player_db[caller_id]);
		}
		else if (event.command.get_command_name() == "사냥") {
			handle_hunt(event, Player_db[caller_id]);
			
		}
		else if (event.command.get_command_name() == "도박") {
			justdice(event, Player_db[caller_id]);
		}
		});

	// 7. 봇 실행
	std::cout << "[시스템] 봇 시작 시도 중..." << std::endl;
	bot.start(dpp::st_wait);

	return 0;
}
