#pragma once


/// 리소스 경로

namespace CL
{
	namespace ResourcePath
	{
		// Data path (리소스 루트)
		const std::string DATA_PATH = "../Data/";

		#pragma region Player
		//메쉬나 애니메이션 데이터는 이름만 있으면 되네(/Data/bin 폴더에 있음)
		
		/// 주무기(장총) Player Rifle
		#pragma region Player_Rifle
		// Mesh
		const std::string MESH_PLAYER_Rifle = "Rifle_Aim.bin";	

		/// None AnimLayer 이름
			const std::string ANIM_LAYER_PLAYER_RIFLE = "Player_Rifle";

			// State 이름
			const std::string ANIM_STATE_PLAYER_RIFLE_STAND_IDLE = "Player_Rifle_Stand_Idle";
			const std::string ANIM_STATE_PLAYER_RIFLE_STAND_WALK_FORWARD = "Player_Rifle_Stand_Walk_Forward";
			const std::string ANIM_STATE_PLAYER_RIFLE_STAND_WALK_BACKWARD = "Player_Rifle_Stand_Walk_Backward";
			const std::string ANIM_STATE_PLAYER_RIFLE_STAND_WALK_LEFT = "Player_Rifle_Stand_Walk_Left";
			const std::string ANIM_STATE_PLAYER_RIFLE_STAND_WALK_RIGHT = "Player_Rifle_Stand_Walk_Right";
			const std::string ANIM_STATE_PLAYER_RIFLE_STAND_SPRINT = "Player_Rifle_Stand_Sprint";
			const std::string ANIM_STATE_PLAYER_RIFLE_STAND_DIE = "Player_Rifle_Stand_Die";
			const std::string ANIM_STATE_PLAYER_RIFLE_STAND_DEAD = "Player_Rifle_Stand_Dead";

			const std::string ANIM_STATE_PLAYER_RIFLE_CROUCH_IDLE = "Player_Rifle_Crouch_Idle";
			const std::string ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_FORWARD = "Player_Rifle_Crouch_Walk_Forward";
			const std::string ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_BACKWARD = "Player_Rifle_Crouch_Walk_Backward";
			const std::string ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_LEFT = "Player_Rifle_Crouch_Walk_Left";
			const std::string ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_RIGHT = "Player_Rifle_Crouch_Walk_Right";


			const std::string ANIM_STATE_PLAYER_RIFLE_SWAPWEAPON = "Player_Rifle_SwapWeapon";
		
		/// Override AnimLayer 이름
			const std::string ANIM_LAYER_PLAYER_RIFLE_AIM = "Player_Rifle_Aim";
			const std::string ANIM_LAYER_PLAYER_RIFLE_STAND_RELOAD = "Rifle_Stand_Reload";
			const std::string ANIM_LAYER_PLAYER_RIFLE_CROUCH_RELOAD = "Rifle_Crouch_Reload";
			const std::string ANIM_LAYER_PLAYER_RIFLE_SWAP_WEAPON = "Player_Rifle_Swap_Weapon";

		/// Animation 이름
			// none (기본 베이스)
			// Stand
			const std::string ANIM_PLAYER_RIFLE_STAND_IDLE = "Rifle_Stand_Stop_mixamo.com.anim";
			const std::string ANIM_PLAYER_RIFLE_STAND_WALK_FORWARD = "Rifle_Stand_FrontMove_mixamo.com.anim";
			const std::string ANIM_PLAYER_RIFLE_STAND_WALK_BACKWARD = "Rifle_Stand_BackwardMove_mixamo.com.anim";
			const std::string ANIM_PLAYER_RIFLE_STAND_WALK_LEFT = "Rifle_Stand_LeftMove_mixamo.com.anim";
			const std::string ANIM_PLAYER_RIFLE_STAND_WALK_RIGHT = "Rifle_Stand_RightMove_mixamo.com.anim";
			const std::string ANIM_PLAYER_RIFLE_STAND_SPRINT = "Rifle_Run_mixamo.com.anim";
			const std::string ANIM_PLAYER_RIFLE_STAND_DIE = "Rifle_Stand_Dying_mixamo.com.anim";
			const std::string ANIM_PLAYER_RIFLE_STAND_DEAD = "Rifle_Stand_Death_mixamo.com.anim";

			// Crouch
			const std::string ANIM_PLAYER_RIFLE_CROUCH_IDLE = "Rifle_Crouch_Stop_mixamo.com.anim";
			const std::string ANIM_PLAYER_RIFLE_CROUCH_WALK_FORWARD = "Rifle_Crouch_FrontMove_mixamo.com.anim";
			const std::string ANIM_PLAYER_RIFLE_CROUCH_WALK_BACKWARD = "Rifle_Crouch_BackwardMove_mixamo.com.anim";
			const std::string ANIM_PLAYER_RIFLE_CROUCH_WALK_LEFT = "Rifle_Crouch_LeftMove_mixamo.com.anim";
			const std::string ANIM_PLAYER_RIFLE_CROUCH_WALK_RIGHT = "Rifle_Crouch_RightMove_mixamo.com.anim";
			
			// override(상체 덮어씌우기)
			// Aim(Pitch)
			const std::string ANIM_PLAYER_RIFLE_PITCH = "Rifle_Aim_mixamo.com.anim";
			//const std::string ANIM_PLAYER_RIFLE_AIM = "Rifle_Aim_Rifle_Aim.anim";
			
			// Reload
			const std::string ANIM_PLAYER_RIFLE_STAND_RELOAD = "Rifle_Stand_Reload_mixamo.com.anim";
			const std::string ANIM_PLAYER_RIFLE_CROUCH_RELOAD = "Rifle_Crouch_Reload_mixamo.com.anim";
		
			// SwapWeapon
			const std::string ANIM_PLAYER_RIFLE_SWAP_WEAPON = "Swap_mixamo.com.anim";

		#pragma endregion Player_Rifle
		

		#pragma endregion Player

		#pragma region Zombie_Runner
		//메쉬나 애니메이션 데이터는 이름만 있으면 되네(/Data/bin 폴더에 있음)
		/// Mesh
		const std::string MESH_Zombie_RUNNER = "RZ_Idle.bin";

		// None AnimLayer 이름
		const std::string ANIM_LAYER_Zombie_1 = "Zombie_1";

			// State 이름
			const std::string ANIM_STATE_ZOMBIE_RUNNER_DIE = "Zombie_1_Die";
			const std::string ANIM_STATE_ZOMBIE_RUNNER_DEAD = "Zombie_1_Dead";
			const std::string ANIM_STATE_ZOMBIE_RUNNER_PATROL = "Zombie_1_Patrol";
			const std::string ANIM_STATE_ZOMBIE_RUNNER_WAIT = "Zombie_1_Wait";
			const std::string ANIM_STATE_ZOMBIE_RUNNER_HUNT = "Zombie_1_Hunt";

		// Animation 이름
			// none (기본 베이스)
			const std::string ANIM_ZOMBIE_RUNNER_DIE = "RZ_HitDying_mixamo.com.anim";
			const std::string ANIM_ZOMBIE_RUNNER_DEAD = "RZ_HitDeath_mixamo.com.anim";
			const std::string ANIM_ZOMBIE_RUNNER_PATROL = "RZ_Move_mixamo.com.anim";
			const std::string ANIM_ZOMBIE_RUNNER_WAIT = "RZ_Idle_mixamo.com.anim";
			const std::string ANIM_ZOMBIE_RUNNER_HUNT = "RZ_Chase_mixamo.com.anim";
			const std::string ANIM_ZOMBIE_RUNNER_ATTACK_PLAYER = "RZ_Attack_mixamo.com.anim";


		#pragma endregion ZombZombie_Runner

		#pragma region UI
		// Image
		const std::string IMAGE_PATH = DATA_PATH + "Image/";

		#pragma endregion UI
	}
}

