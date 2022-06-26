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
		const std::string MESH_PLAYER_RIFLE = "Rifle_Aim.bin";	

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
		
		/// None AnimLayer 이름
		const std::string ANIM_LAYER_PLAYER_RIFLE_ASSASSINATE_BEGIN = "Player_Rifle_Assassinate_Begin";
		const std::string ANIM_LAYER_PLAYER_RIFLE_ASSASSINATE_END = "Player_Rifle_Assassinate_End";

		/// Override AnimLayer 이름
			const std::string ANIM_LAYER_PLAYER_RIFLE_AIM = "Player_Rifle_Aim";
			const std::string ANIM_LAYER_PLAYER_RIFLE_STAND_RELOAD = "Player_Rifle_Stand_Reload";
			const std::string ANIM_LAYER_PLAYER_RIFLE_CROUCH_RELOAD = "Player_Rifle_Crouch_Reload";
			const std::string ANIM_LAYER_PLAYER_RIFLE_SWAP_WEAPON = "Player_Rifle_Swap_Weapon";
			
			const std::string ANIM_LAYER_PLAYER_RIFLE_ROUTING_ITEM = "Player_Rifle_RoutingItem";
			const std::string ANIM_LAYER_PLAYER_RIFLE_INTERACTION = "Player_Rifle_Interaction";

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
			
			// Reload
			const std::string ANIM_PLAYER_RIFLE_STAND_RELOAD = "Rifle_Stand_Reload_mixamo.com.anim";
			const std::string ANIM_PLAYER_RIFLE_CROUCH_RELOAD = "Rifle_Crouch_Reload_mixamo.com.anim";
		
			// SwapWeapon
			const std::string ANIM_PLAYER_RIFLE_SWAP_WEAPON = "Rifle_Common_Swap_mixamo.com.anim";

			// FFimctopm
			// Assassinate
			const std::string ANIM_PLAYER_RIFLE_ASSASSINATE_BEGIN = "Rifle_Common_Assassinate_01_mixamo.com.anim";
			const std::string ANIM_PLAYER_RIFLE_ASSASSINATE_END = "Rifle_Common_Assassinate_02_mixamo.com.anim";

			// RoutingItem
			const std::string ANIM_PLAYER_RIFLE_ROUTING_ITEM = "Rifle_Common_Looting_mixamo.com.anim";

			// Interaction
			//const std::string ANIM_PLAYER_RIFLE_INTERACTION = "Rifle_Common_Swap_mixamo.com.anim";

		#pragma endregion Player_Rifle
		
		#pragma region Player_Pistole
			// Mesh
			const std::string MESH_PLAYER_PISTOL = "Pistol_Aim.bin";

			/// None AnimLayer 이름
			const std::string ANIM_LAYER_PLAYER_PISTOL = "Player_Pistol";

				// State 이름
				const std::string ANIM_STATE_PLAYER_PISTOL_STAND_IDLE = "Player_Pistol_Stand_Idle";
				const std::string ANIM_STATE_PLAYER_PISTOL_STAND_WALK_FORWARD = "Player_Pistol_Stand_Walk_Forward";
				const std::string ANIM_STATE_PLAYER_PISTOL_STAND_WALK_BACKWARD = "Player_Pistol_Stand_Walk_Backward";
				const std::string ANIM_STATE_PLAYER_PISTOL_STAND_WALK_LEFT = "Player_Pistol_Stand_Walk_Left";
				const std::string ANIM_STATE_PLAYER_PISTOL_STAND_WALK_RIGHT = "Player_Pistol_Stand_Walk_Right";
				const std::string ANIM_STATE_PLAYER_PISTOL_STAND_SPRINT = "Player_Pistol_Stand_Sprint";
				const std::string ANIM_STATE_PLAYER_PISTOL_STAND_DIE = "Player_Pistol_Stand_Die";
				const std::string ANIM_STATE_PLAYER_PISTOL_STAND_DEAD = "Player_Pistol_Stand_Dead";

				const std::string ANIM_STATE_PLAYER_PISTOL_CROUCH_IDLE = "Player_Pistol_Crouch_Idle";
				const std::string ANIM_STATE_PLAYER_PISTOL_CROUCH_WALK_FORWARD = "Player_Pistol_Crouch_Walk_Forward";
				const std::string ANIM_STATE_PLAYER_PISTOL_CROUCH_WALK_BACKWARD = "Player_Pistol_Crouch_Walk_Backward";
				const std::string ANIM_STATE_PLAYER_PISTOL_CROUCH_WALK_LEFT = "Player_Pistol_Crouch_Walk_Left";
				const std::string ANIM_STATE_PLAYER_PISTOL_CROUCH_WALK_RIGHT = "Player_Pistol_Crouch_Walk_Right";

			/// None AnimLayer 이름
			const std::string ANIM_LAYER_PLAYER_PISTOL_ASSASSINATE_BEGIN = "Player_Pistol_Assassinate_Begin";
			const std::string ANIM_LAYER_PLAYER_PISTOL_ASSASSINATE_END = "Player_Pistol_Assassinate_End";

			/// Override AnimLayer 이름
			const std::string ANIM_LAYER_PLAYER_PISTOL_AIM = "Player_Pistol_Aim";
			const std::string ANIM_LAYER_PLAYER_PISTOL_STAND_RELOAD = "Player_Pistol_Stand_Reload";
			const std::string ANIM_LAYER_PLAYER_PISTOL_CROUCH_RELOAD = "Player_Pistol_Crouch_Reload";
			const std::string ANIM_LAYER_PLAYER_PISTOL_SWAP_WEAPON = "Player_Pistol_Swap_Weapon";


			/// Animation 이름
			// none (기본 베이스)
			// Stand
			const std::string ANIM_PLAYER_PISTOL_STAND_IDLE = "Pistol_Stand_Stop_mixamo.com.anim";
			const std::string ANIM_PLAYER_PISTOL_STAND_WALK_FORWARD = "Pistol_Stand_FrontMove_mixamo.com.anim";
			const std::string ANIM_PLAYER_PISTOL_STAND_WALK_BACKWARD = "Pistol_Stand_BackwardMove_mixamo.com.anim";
			const std::string ANIM_PLAYER_PISTOL_STAND_WALK_LEFT = "Pistol_Stand_LeftMove_mixamo.com.anim";
			const std::string ANIM_PLAYER_PISTOL_STAND_WALK_RIGHT = "Pistol_Stand_RightMove_mixamo.com.anim";
			const std::string ANIM_PLAYER_PISTOL_STAND_SPRINT = "Pistol_Common_Sprint_mixamo.com.anim";
			const std::string ANIM_PLAYER_PISTOL_STAND_DIE = "Rifle_Stand_Dying_mixamo.com.anim";
			const std::string ANIM_PLAYER_PISTOL_STAND_DEAD = "Rifle_Stand_Death_mixamo.com.anim";

			// Crouch
			const std::string ANIM_PLAYER_PISTOL_CROUCH_IDLE = "Pistol_Crouch_Stop_mixamo.com.anim";
			const std::string ANIM_PLAYER_PISTOL_CROUCH_WALK_FORWARD = "Pistol_Crouch_FrontMove_mixamo.com.anim";
			const std::string ANIM_PLAYER_PISTOL_CROUCH_WALK_BACKWARD = "Pistol_Crouch_BackwardMove_mixamo.com.anim";
			const std::string ANIM_PLAYER_PISTOL_CROUCH_WALK_LEFT = "Pistol_Crouch_LeftMove_mixamo.com.anim";
			const std::string ANIM_PLAYER_PISTOL_CROUCH_WALK_RIGHT = "Pistol_Crouch_RightMove_mixamo.com.anim";

			// override(상체 덮어씌우기)
			// Aim(Pitch)
			const std::string ANIM_PLAYER_PISTOL_PITCH = "Pistol_Aim_mixamo.com.anim";

			// SwapWeapon
			const std::string ANIM_PLAYER_PISTOL_SWAP_WEAPON = "Pistol_Swap_mixamo.com.anim";

			/// FFimctopm
			// Assassinate
			const std::string ANIM_PLAYER_PISTOL_ASSASSINATE_BEGIN = "Pistol_Common_Assassinate_01_mixamo.com.anim";
			const std::string ANIM_PLAYER_PISTOL_ASSASSINATE_END = "Pistol_Common_Assassinate_02_mixamo.com.anim";

		#pragma endregion Player_Pistole


		#pragma endregion Player

		#pragma region Partner
			// Mesh
			const std::string MESH_PARTNER = "Stand_Stop__Standing Idle.bin";

			/// None AnimLayer 이름
			const std::string ANIM_LAYER_PARTNER_MOVENT = "Partner_Movent";
				
				// State 이름
				const std::string ANIM_STATE_PARTNER_STAND_IDLE = "Partner_Movement_Stand_Idle";
				const std::string ANIM_STATE_PARTNER_STAND_WALK = "Partner_Movement_Stand_Walk";
				const std::string ANIM_STATE_PARTNER_STAND_SPRINT = "Partner_Movement_Stand_SPRINT";
					
				const std::string ANIM_STATE_PARTNER_CROUCH_IDLE = "Partner_Movement_Crouch_Idle";
				const std::string ANIM_STATE_PARTNER_CROUCH_WALK = "Partner_Movement_Crouch_Walk";

				const std::string ANIM_STATE_PARTNER_HIT_BEGIN = "Partner_Hit_Begin";
				const std::string ANIM_STATE_PARTNER_HIT_MIDDLE = "Partner_Hit_Middle";
				const std::string ANIM_STATE_PARTNER_HIT_END = "Partner_Hit_End";

				const std::string ANIM_STATE_PARTNER_DIE = "Partner_Die";
				const std::string ANIM_STATE_PARTNER_DEAD = "Partner_Dead";

			/// Animation 이름
			// none (기본 베이스)
			const std::string ANIM_PARTNER_STAND_IDLE = "Stand_Stop__Standing Idle_mixamo.com.anim";
			const std::string ANIM_PARTNER_STAND_WALK = "Stand_Move__Standing Walk Forward_mixamo.com.anim";
			const std::string ANIM_PARTNER_STAND_SPRINT = "Sprint__Slow Run_mixamo.com.anim";
			const std::string ANIM_PARTNER_CROUCH_IDLE = "Crouch_Stop__Crouching Idle_mixamo.com.anim";
			const std::string ANIM_PARTNER_CROUCH_WALK = "Crouch_Move__Crouched Walking_mixamo.com.anim";
			
			
			const std::string ANIM_PARTNER_HIT_BEGIN = "Be Threat_01__Taken Hostage - Victim_mixamo.com.anim";
			const std::string ANIM_PARTNER_HIT_MIDDLE = "Be Threat_02__Taken Hostage - Victim_mixamo.com.anim";
			const std::string ANIM_PARTNER_HIT_END = "Be Threat_03__Release Hostage - Hostage_mixamo.com.anim";
			const std::string ANIM_PARTNER_DIE = "Death__Standing Death Backward 01_mixamo.com.anim";
			const std::string ANIM_PARTNER_DEAD = "Dead__Standing Death Backward 01_mixamo.com.anim";

		#pragma endregion Partner

		#pragma region Zombie_Runner
		//메쉬나 애니메이션 데이터는 이름만 있으면 되네(/Data/bin 폴더에 있음)
		/// Mesh
		const std::string MESH_Zombie_RUNNER = "RZ_Idle__Zombie Scratch Idle.bin";

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
			const std::string ANIM_ZOMBIE_RUNNER_DIE = "RZ_HitDeath__Zombie Death_mixamo.com.anim";
			const std::string ANIM_ZOMBIE_RUNNER_DEAD = "RZ_HitDead__Zombie Death_mixamo.com.anim";
			const std::string ANIM_ZOMBIE_RUNNER_PATROL = "RZ_Move__Zombie Walk_mixamo.com.anim";
			const std::string ANIM_ZOMBIE_RUNNER_WAIT = "RZ_Idle__Zombie Scratch Idle_mixamo.com.anim";
			const std::string ANIM_ZOMBIE_RUNNER_HUNT = "RZ_Chase__Zombie Running_mixamo.com.anim";
			const std::string ANIM_ZOMBIE_RUNNER_ATTACK_PLAYER = "RZ_Attack__Zombie Attack_mixamo.com.anim";
			const std::string ANIM_ZOMBIE_RUNNER_ATTACK_PARTNER_BEGIN = "RZ_PartnerAttackBegin__Zombie Neck Bite_mixamo.com.anim";
			const std::string ANIM_ZOMBIE_RUNNER_ATTACK_PARTNER_MIDDLE = "RZ_PartnerAttacking__Zombie Neck Bite_mixamo.com.anim";
			const std::string ANIM_ZOMBIE_RUNNER_ATTACK_PARTNER_END = "RZ_PartnerAttackEnd__Zombie Neck Bite_mixamo.com.anim";
			const std::string ANIM_ZOMBIE_RUNNER_AWAKEN_SIGHT = "RZ_SightFind__Zombie Scream_mixamo.com.anim";
			const std::string ANIM_ZOMBIE_RUNNER_AWAKEN_SOUNED = "RZ_SoundFind__Zombie Transition_mixamo.com.anim";
			const std::string ANIM_ZOMBIE_RUNNER_ASSASSINATED = "RZ_StealthDeath__Taken Hostage - Victim_mixamo.com.anim";

			

		#pragma endregion ZombZombie_Runner

		#pragma region UI
		// Image
		const std::string IMAGE_PATH = DATA_PATH + "Image/";

		#pragma endregion UI
	}
}

