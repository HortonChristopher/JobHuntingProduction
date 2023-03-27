#include "Menu.h"

int SettingParameters::padSensitivity = 3;
int SettingParameters::reverseX = 1;
int SettingParameters::reverseY = 1;

//int SettingParameters::jumpButton = XINPUT_GAMEPAD_A;
//int SettingParameters::airSlideButton = XINPUT_GAMEPAD_B;
int SettingParameters::runButton = XINPUT_GAMEPAD_RIGHT_SHOULDER;
//int SettingParameters::resetButton = XINPUT_GAMEPAD_RIGHT_SHOULDER;
bool SettingParameters::viewCollision = false;

extern DeltaTime* deltaTime;

Menu::Menu()
{

}

Menu::~Menu()
{

}

void Menu::Initialize(DirectXCommon* dxCommon, Input* input)
{
	// Checking for nullptr
	assert(dxCommon);
	assert(input);

	// Assigning variables to this
	this->dxCommon = dxCommon;
	this->input = input;

	if (!Sprite::LoadTexture(701, "menuUI.png")) { assert(0); return; }
	if (!Sprite::LoadTexture(702, "setting.png")) { assert(0); return; }
	if (!Sprite::LoadTexture(703, "end.png")) { assert(0); return; }
	if (!Sprite::LoadTexture(704, "sensitiveSetting.png")) { assert(0); return; }
	if (!Sprite::LoadTexture(705, "operateSetting.png")) { assert(0); return; }
	if (!Sprite::LoadTexture(706, "ScreenSetting.png")) { assert(0); return; }
	if (!Sprite::LoadTexture(707, "upsideDown.png")) { assert(0); return; }
	if (!Sprite::LoadTexture(708, "check.png")) { assert(0); return; }
	if (!Sprite::LoadTexture(709, "flipLeftRight.png")) { assert(0); return; }
	if (!Sprite::LoadTexture(710, "a.png")) { assert(0); return; }
	if (!Sprite::LoadTexture(711, "cameraReset.png")) { assert(0); return; }
	if (!Sprite::LoadTexture(712, "action.png")) { assert(0); return; }
	if (!Sprite::LoadTexture(713, "changeRun.png")) { assert(0); return; }
	if (!Sprite::LoadTexture(714, "selectFrame.png")) { assert(0); return; }
	if (!Sprite::LoadTexture(715, "dot.png")) { assert(0); return; }
	if (!Sprite::LoadTexture(716, "sensitiveSettingBar.png")) { assert(0); return; }
	if (!Sprite::LoadTexture(717, "ScreenSetting.png")) { assert(0); return; }
	if (!Sprite::LoadTexture(718, "viewCollision.png")) { assert(0); return; }
	if (!Sprite::LoadTexture(719, "sensitiveSettingBar.png")) { assert(0); return; }
	if (!Sprite::LoadTexture(720, "endCheck.png")) { assert(0); return; }
	if (!Sprite::LoadTexture(721, "yes.png")) { assert(0); return; }
	if (!Sprite::LoadTexture(722, "no.png")) { assert(0); return; }
	if (!Sprite::LoadTexture(723, "b.png")) { assert(0); return; }
	if (!Sprite::LoadTexture(724, "x.png")) { assert(0); return; }
	if (!Sprite::LoadTexture(725, "y.png")) { assert(0); return; }
	if (!Sprite::LoadTexture(726, "LT.png")) { assert(0); return; }
	if (!Sprite::LoadTexture(727, "RT.png")) { assert(0); return; }
	if (!Sprite::LoadTexture(728, "LB.png")) { assert(0); return; }
	if (!Sprite::LoadTexture(729, "RB.png")) { assert(0); return; }

	easingScale = 0;
	openCounter = 0;
	menuUIScale = { 0,0 };
	backTexScale = { 1,1 };
	restartScale = { 1,1 };
	endScale = { 1,1 };
	settingScale = { 1.5f,1.5f };
	sensitiveSettingScale = { 1.5f,1.5f };
	operateSettingScale = { 1,1 };
	yesScale = { 1.5f,1.5f };
	noScale = { 1,1 };
	screenSettingScale = { 1,1 };
	select = 0;
	close = false;
	startClose = false;
	closeCounter = 0;
	menuState = Base;
	dotPos = { 480 - 228 * 1.5f,300 - 20 * 1.5f };
	selectFramePos = {};
	operateChangeFlag = false;
	returnTitle = false;
	texAlpha = 1;
	migrateCounter = 0;
	migrate = false;

	menuUI = Sprite::Create(701, { 480.0f, 270.0f });
	setting = Sprite::Create(702, { 480.0f, 104.0f });
	end = Sprite::Create(703, { 480.0f, 436.0f });
	sensitivitySetting = Sprite::Create(704, { 480.0f, 104.0f });
	operateSetting = Sprite::Create(705, { 480.0f, 270.0f });
	screenSetting = Sprite::Create(706, { 480.0f, 436.0f });
	upsideDown = Sprite::Create(707, { 295.0f, 200.0f });
	check = Sprite::Create(708, { 403.0f, 196.0f });
	flipLeftRight = Sprite::Create(709, { 655.0f, 200.0f });
	cameraReset = Sprite::Create(711, { 620.0f, 300.0f });
	action = Sprite::Create(712, { 240.0f, 400.0f });
	changeRun = Sprite::Create(713, { 620.0f, 400.0f });
	check2 = Sprite::Create(708, { 720.0f, 390.0f });
	selectFrame = Sprite::Create(714, selectFramePos);
	dot = Sprite::Create(715, dotPos);
	sensitiveSettingBar = Sprite::Create(716, { 480.0f, 300.0f });
	screenSetting = Sprite::Create(717, { 480.0f, 105.0f });
	viewCollision = Sprite::Create(718, { 480.0f, 240.0f });
	endCheck = Sprite::Create(720, { 480.0f, 150.0f });
	yes = Sprite::Create(721, { 320.0f, 380.0f });
	no = Sprite::Create(722, { 640.0f, 380.0f });
}

bool Menu::Update()
{
	//表示状態
	if (openCounter > 30)
	{
		//クローズ時の縮小処理
		if (startClose)
		{
			if (closeCounter > 30)
			{
				close = true;
			}
			easingScale = Easing::EaseInOutQuartic(1.0f, 0.0f, 30.0f, (float)closeCounter);
			menuUIScale = { easingScale,easingScale };
			closeCounter++;
		}
		else
		{
			//遷移時は遷移処理のみで操作を受け付けない
			if (migrate)
			{
				Migrate();
				return false;
			}
			//メニュー選択画面の処理（メニュー本体）
			switch (menuState)
			{
			case Base:
				BaseMenu();
				break;

			case SettingSelect:
				Setting();
				break;

			case PadOperateSetting:
				OperateSetting();
				break;

			case PadSensitiveSetting:
				SensitiveSetting();
				break;

			case ScreenSetting:
				ScreenSettingProcess();
				break;

			//case RestartCheck:
				//RestartCheckProcess();
				//break;

			case EndCheck:
				EndCheckProcess();
				break;

			default:
				break;
			}
			//どの画面でもメニューボタンが押されたら閉じる
			if (input->TriggerKey(DIK_ESCAPE) || input->TriggerControllerButton(XINPUT_GAMEPAD_START))
				startClose = true;
		}
	}
	//メニュー開いたときの拡大処理
	else
	{
		easingScale = Easing::EaseInOutQuartic(0.0f, 1.0f, 30.0f, (float)openCounter);
		menuUIScale = { easingScale,easingScale };
		openCounter++;
	}
	return close;
}

void Menu::Draw()
{
	// Setting command list
	ID3D12GraphicsCommandList* cmdList = dxCommon->GetCommandList();

	//リソースバリアの変更とレンダーターゲットをメニュー用のテクスチャに変更
	Sprite::PreDraw(cmdList);

	menuUI->Draw();
	//各画面状態に合わせて描画が内容を変更
	switch (menuState)
	{
	case Base:
		setting->Draw();
		setting->SetColor({ 1.0f, 1.0f, 1.0f, texAlpha });
		end->Draw();
		end->SetColor({ 1.0f, 1.0f, 1.0f, texAlpha });
		break;
	case SettingSelect:
		sensitivitySetting->Draw();
		sensitivitySetting->SetColor({ 1.0f, 1.0f, 1.0f, texAlpha });
		operateSetting->Draw();
		operateSetting->SetColor({ 1.0f, 1.0f, 1.0f, texAlpha });
		screenSetting->Draw();
		screenSetting->SetColor({ 1.0f, 1.0f, 1.0f, texAlpha });
		screenSetting->SetPosition({ 480.0f, 436.0f });
		screenSetting->SetColor({ 1.0f, 1.0f, 1.0f, texAlpha });
		break;

	case PadOperateSetting:
		operateSetting->Draw();
		operateSetting->SetColor({ 1.0f, 1.0f, 1.0f, texAlpha });

		upsideDown->Draw();
		upsideDown->SetColor({ 1.0f, 1.0f, 1.0f, texAlpha });
		//ONの時のみチェックマークを描画
		if (SettingParameters::reverseY == -1)
		{
			check->Draw();
			check->SetPosition({ 403.0f, 196.0f });
			check->SetColor({ 1.0f, 1.0f, 1.0f, texAlpha });
		}

		flipLeftRight->Draw();
		flipLeftRight->SetColor({ 1.0f, 1.0f, 1.0f, texAlpha });

		//ONの時のみチェックマークを描画
		if (SettingParameters::reverseX == -1)
		{
			check2->Draw();
			check2->SetPosition({ 763.0f, 196.0f });
			check2->SetColor({ 1.0f, 1.0f, 1.0f, texAlpha });
		}

		cameraReset->Draw();
		cameraReset->SetColor({ 1.0f, 1.0f, 1.0f, texAlpha });
		//cameraResetOperate->DrawSprite(operateTexNames[SettingParam::resetButton], { 830,300 }, 0, { 1,1 }, { 1,1,1,texAlpha });
		action->Draw();
		action->SetColor({ 1.0f, 1.0f, 1.0f, texAlpha });
		//actionOperate->DrawSprite(operateTexNames[SettingParam::airSlideButton], { 400,400 }, 0, { 1,1 }, { 1,1,1,texAlpha });
		changeRun->Draw();
		changeRun->SetColor({ 1.0f, 1.0f, 1.0f, texAlpha });
		//changeRunOperate->DrawSprite(operateTexNames[SettingParam::runButton], { 830,400 }, 0, { 1,1 }, { 1,1,1,texAlpha });
		selectFrame->Draw();
		selectFrame->SetPosition(selectFramePos);
		selectFrame->SetColor({ 1.0f, 1.0f, 1.0f, texAlpha });
		break;

	case PadSensitiveSetting:
		sensitivitySetting->Draw();
		sensitivitySetting->SetColor({ 1.0f, 1.0f, 1.0f, texAlpha });
		dot->Draw();
		dot->SetPosition(dotPos);
		dot->SetColor({ 1.0f, 1.0f, 1.0f, texAlpha });
		sensitiveSettingBar->Draw();
		sensitiveSettingBar->SetColor({ 1.0f, 1.0f, 1.0f, texAlpha });
		break;

	case ScreenSetting:
		screenSetting->Draw();
		screenSetting->SetPosition({ 480.0f, 105.0f });
		screenSetting->SetColor({ 1.0f, 1.0f, 1.0f, texAlpha });
		viewCollision->Draw();
		viewCollision->SetColor({ 1.0f, 1.0f, 1.0f, texAlpha });

		//ONの時のみチェックマークを描画
		if (SettingParameters::viewCollision)
		{
			check->Draw();
			check->SetPosition({ 730.0f, 240.0f });
			check->SetColor({ 1.0f, 1.0f, 1.0f, texAlpha });
		}

		selectFrame->Draw();
		selectFrame->SetColor({ 1.0f, 1.0f, 1.0f, texAlpha });
		break;

	case EndCheck:
		endCheck->Draw();
		endCheck->SetColor({ 1.0f, 1.0f, 1.0f, texAlpha });
		yes->Draw();
		yes->SetColor({ 1.0f, 1.0f, 1.0f, texAlpha });
		no->Draw();
		no->SetColor({ 1.0f, 1.0f, 1.0f, texAlpha });
		break;

	default:
		break;
	}
	// Sprite post draw
	Sprite::PostDraw();
}

void Menu::BaseMenu()
{
	if (input->TriggerLStickDown() || input->TriggerControllerButton(XINPUT_GAMEPAD_DPAD_DOWN)
		|| input->TriggerLStickUp() || input->TriggerControllerButton(XINPUT_GAMEPAD_DPAD_UP)
		|| input->TriggerKey(DIK_W) || input->TriggerKey(DIK_S))
	{
		if (input->TriggerLStickDown() || input->TriggerControllerButton(XINPUT_GAMEPAD_DPAD_DOWN) || input->TriggerKey(DIK_S))
		{
			if (select < 2)
				select++;
		}
		if (input->TriggerLStickUp() || input->TriggerControllerButton(XINPUT_GAMEPAD_DPAD_UP) || input->TriggerKey(DIK_W))
		{
			if (select > 0)
				select--;
		}
		SetTexParam();
	}

	if (input->TriggerControllerButton(XINPUT_GAMEPAD_B) || input->TriggerKey(DIK_SPACE))
	{
		switch (select)
		{
		case 0:
			nextMenuState = SettingSelect;
			break;
		case 1:
			break;
		case 2:
			nextMenuState = EndCheck;
			break;
		default:
			return;
		}
		migrate = true;
		SetTexParam();
	}

	if (input->TriggerControllerButton(XINPUT_GAMEPAD_A) || input->TriggerKey(DIK_Z))
	{
		startClose = true;
	}
}

void Menu::Setting()
{
	if (input->TriggerLStickDown() || input->TriggerControllerButton(XINPUT_GAMEPAD_DPAD_DOWN)
		|| input->TriggerLStickUp() || input->TriggerControllerButton(XINPUT_GAMEPAD_DPAD_UP)
		|| input->TriggerKey(DIK_W) || input->TriggerKey(DIK_S))
	{
		if (input->TriggerLStickDown() || input->TriggerControllerButton(XINPUT_GAMEPAD_DPAD_DOWN) || input->TriggerKey(DIK_S))
		{
			if (select < 2)
				select++;
		}
		if (input->TriggerLStickUp() || input->TriggerControllerButton(XINPUT_GAMEPAD_DPAD_UP) || input->TriggerKey(DIK_W))
		{
			if (select > 0)
				select--;
		}
		SetTexParam();
	}
	if (input->TriggerControllerButton(XINPUT_GAMEPAD_B) || input->TriggerKey(DIK_SPACE))
	{
		switch (select)
		{
		case 0:
			nextMenuState = PadSensitiveSetting;
			break;
		case 1:
			nextMenuState = PadOperateSetting;
			break;
		case 2:
			nextMenuState = ScreenSetting;
			break;
		default:
			return;
		}
		migrate = true;
		SetTexParam();
	}
	if (input->TriggerControllerButton(XINPUT_GAMEPAD_A) || input->TriggerKey(DIK_Z))
	{
		migrate = true;
		nextMenuState = Base;
		SetTexParam();
	}
}

void Menu::SensitiveSetting()
{
	if (input->TriggerMouseLeft() || input->TriggerControllerButton(XINPUT_GAMEPAD_DPAD_LEFT)
		|| input->TriggerLStickRight() || input->TriggerControllerButton(XINPUT_GAMEPAD_DPAD_RIGHT)
		|| input->TriggerKey(DIK_A) || input->TriggerKey(DIK_D))

	{
		if (input->TriggerLStickRight() || input->TriggerControllerButton(XINPUT_GAMEPAD_DPAD_RIGHT) || input->TriggerKey(DIK_D))
		{
			if (SettingParameters::padSensitivity < 10)
				SettingParameters::padSensitivity++;
		}
		if (input->TriggerLStickLeft() || input->TriggerControllerButton(XINPUT_GAMEPAD_DPAD_LEFT) || input->TriggerKey(DIK_A))
		{
			if (SettingParameters::padSensitivity > 1)
				SettingParameters::padSensitivity--;
		}
		SetTexParam();
	}
	if (input->TriggerControllerButton(XINPUT_GAMEPAD_A) || input->TriggerKey(DIK_Z))
	{
		nextMenuState = SettingSelect;
		migrate = true;
		SetTexParam();
	}
}

void Menu::OperateSetting()
{
	if (operateChangeFlag)
	{
		//何かしらのPADのボタンが押されたら代入される
		int button = input->CheckControllerButton();

		//押されたボタンが割り当て可能範囲内のボタンだったら
		if (button == XINPUT_GAMEPAD_LEFT_THUMB ||
			button == XINPUT_GAMEPAD_RIGHT_THUMB ||
			button == XINPUT_GAMEPAD_LEFT_SHOULDER ||
			button == XINPUT_GAMEPAD_RIGHT_SHOULDER ||
			button == XINPUT_GAMEPAD_A ||
			button == XINPUT_GAMEPAD_B ||
			button == XINPUT_GAMEPAD_X ||
			button == XINPUT_GAMEPAD_Y)
		{
			//変更対象によって場合分け＋ボタンの変更＋変更後のボタンが既に他の操作に割り当て済みの場合ボタンを入れ替える
			/*switch (select)
			{
			case 2:
				if (SettingParameters::resetButton == button) SettingParameters::resetButton = SettingParameters::jumpButton;

				else if (SettingParameters::airSlideButton == button) SettingParameters::airSlideButton = SettingParameters::jumpButton;

				else if (SettingParameters::runButton == button) SettingParameters::runButton = SettingParameters::jumpButton;

				SettingParameters::jumpButton = button;
				break;
			case 3:
				if (SettingParameters::jumpButton == button) SettingParameters::jumpButton = SettingParameters::resetButton;

				else if (SettingParameters::airSlideButton == button) SettingParameters::airSlideButton = SettingParameters::resetButton;

				else if (SettingParameters::runButton == button) SettingParameters::runButton = SettingParameters::resetButton;

				SettingParameters::resetButton = button;
				break;
			case 4:
				if (SettingParameters::jumpButton == button) SettingParameters::jumpButton = SettingParameters::airSlideButton;

				else if (SettingParameters::resetButton == button) SettingParameters::resetButton = SettingParameters::airSlideButton;

				else if (SettingParameters::runButton == button) SettingParameters::runButton = SettingParameters::airSlideButton;

				SettingParameters::airSlideButton = button;
				break;
			case 5:
				if (SettingParameters::jumpButton == button) SettingParameters::jumpButton = SettingParameters::runButton;

				else if (SettingParameters::resetButton == button) SettingParameters::resetButton = SettingParameters::runButton;

				else if (SettingParameters::airSlideButton == button) SettingParameters::airSlideButton = SettingParameters::runButton;

				SettingParameters::runButton = button;
				break;
			default:
				return;
			}
			operateChangeFlag = false;*/
		}
		return;
	}
	if (input->TriggerLStickDown() || input->TriggerControllerButton(XINPUT_GAMEPAD_DPAD_DOWN)
		|| input->TriggerLStickUp() || input->TriggerControllerButton(XINPUT_GAMEPAD_DPAD_UP) ||
		input->TriggerLStickLeft() || input->TriggerControllerButton(XINPUT_GAMEPAD_DPAD_LEFT)
		|| input->TriggerLStickRight() || input->TriggerControllerButton(XINPUT_GAMEPAD_DPAD_RIGHT)
		|| input->TriggerKey(DIK_A) || input->TriggerKey(DIK_D) || input->TriggerKey(DIK_W) || input->TriggerKey(DIK_S))
	{
		if (input->TriggerLStickRight() || input->TriggerControllerButton(XINPUT_GAMEPAD_DPAD_RIGHT) || input->TriggerKey(DIK_D))
		{
			if (select < 5)
				select++;
		}
		if (input->TriggerLStickLeft() || input->TriggerControllerButton(XINPUT_GAMEPAD_DPAD_LEFT) || input->TriggerKey(DIK_A))
		{
			if (select > 0)
				select--;
		}
		if (input->TriggerLStickDown() || input->TriggerControllerButton(XINPUT_GAMEPAD_DPAD_DOWN) || input->TriggerKey(DIK_S))
		{
			if (select < 5)
			{
				select += 2;
				if (select > 5)
					select = 5;
			}
		}
		if (input->TriggerLStickUp() || input->TriggerControllerButton(XINPUT_GAMEPAD_DPAD_UP) || input->TriggerKey(DIK_W))
		{
			if (select > 0)
			{
				select -= 2;
				if (select < 0)
					select = 0;
			}
		}
		SetTexParam();
	}
	if (input->TriggerControllerButton(XINPUT_GAMEPAD_B) || input->TriggerKey(DIK_SPACE))
	{
		switch (select)
		{
		case 0:
			SettingParameters::reverseY = SettingParameters::reverseY == 1 ? -1 : 1;
			break;
		case 1:
			SettingParameters::reverseX = SettingParameters::reverseX == 1 ? -1 : 1;
			break;
		case 2:
		case 3:
		case 4:
		case 5:
			operateChangeFlag = true;
			break;
		default:
			return;
		}
		SetTexParam();
	}
	if (input->TriggerControllerButton(XINPUT_GAMEPAD_A) || input->TriggerKey(DIK_Z))
	{
		nextMenuState = SettingSelect;
		migrate = true;
		SetTexParam();
	}
}

void Menu::ScreenSettingProcess()
{
	if (input->TriggerLStickDown() || input->TriggerControllerButton(XINPUT_GAMEPAD_DPAD_DOWN)
		|| input->TriggerLStickUp() || input->TriggerControllerButton(XINPUT_GAMEPAD_DPAD_UP)
		|| input->TriggerKey(DIK_W) || input->TriggerKey(DIK_S))
	{
		if (input->TriggerLStickDown() || input->TriggerControllerButton(XINPUT_GAMEPAD_DPAD_DOWN) || input->TriggerKey(DIK_S))
		{
			if (select < 1)
				select++;
		}
		if (input->TriggerLStickUp() || input->TriggerControllerButton(XINPUT_GAMEPAD_DPAD_UP) || input->TriggerKey(DIK_W))
		{
			if (select > 0)
				select--;
		}
		SetTexParam();
	}
	if (input->TriggerControllerButton(XINPUT_GAMEPAD_B) || input->TriggerKey(DIK_SPACE))
	{
		switch (select)
		{
		case 0:
			SettingParameters::viewCollision = SettingParameters::viewCollision == true ? false : true;
			break;
		case 1:
			break;
		default:
			return;
		}
		SetTexParam();
	}
	if (input->TriggerControllerButton(XINPUT_GAMEPAD_A) || input->TriggerKey(DIK_Z))
	{
		migrate = true;
		nextMenuState = SettingSelect;
		SetTexParam();
	}
}

void Menu::EndCheckProcess()
{
	if (input->TriggerLStickLeft() || input->TriggerControllerButton(XINPUT_GAMEPAD_DPAD_LEFT)
		|| input->TriggerLStickRight() || input->TriggerControllerButton(XINPUT_GAMEPAD_DPAD_RIGHT)
		|| input->TriggerKey(DIK_A) || input->TriggerKey(DIK_D))
	{
		if (input->TriggerLStickRight() || input->TriggerControllerButton(XINPUT_GAMEPAD_DPAD_RIGHT) || input->TriggerKey(DIK_D))
		{
			if (select < 1)
				select++;
		}
		if (input->TriggerLStickLeft() || input->TriggerControllerButton(XINPUT_GAMEPAD_DPAD_LEFT) || input->TriggerKey(DIK_A))
		{
			if (select > 0)
				select--;
		}
		SetTexParam();
	}

	if (input->TriggerControllerButton(XINPUT_GAMEPAD_B) || input->TriggerKey(DIK_SPACE))
	{
		switch (select)
		{
		case 0:
			returnTitle = true;
			startClose = true;
			break;
		case 1:
			migrate = true;
			nextMenuState = Base;
			break;
		default:
			return;
		}
		SetTexParam();
	}
	if (input->TriggerControllerButton(XINPUT_GAMEPAD_A) || input->TriggerKey(DIK_Z))
	{
		migrate = true;
		nextMenuState = Base;
		SetTexParam();
	}
}

void Menu::SetTexParam()
{
	switch (menuState)
	{
	case Base:
		switch (select)
		{
		case 0:
			restartScale = { 1,1 };
			endScale = { 1,1 };
			settingScale = { 1.5f,1.5f };
			break;
		case 1:
			restartScale = { 1.5f,1.5f };
			endScale = { 1,1 };
			settingScale = { 1,1 };
			break;
		case 2:
			restartScale = { 1,1 };
			endScale = { 1.5f,1.5f };
			settingScale = { 1,1 };
			break;
		default:
			break;
		}
		break;
	case SettingSelect:
		switch (select)
		{
		case 0:
			sensitiveSettingScale = { 1.5f,1.5f };
			operateSettingScale = { 1,1 };
			screenSettingScale = { 1,1 };
			break;
		case 1:
			operateSettingScale = { 1.5f,1.5f };
			sensitiveSettingScale = { 1,1 };
			screenSettingScale = { 1,1 };
			break;
		case 2:
			operateSettingScale = { 1,1 };
			sensitiveSettingScale = { 1,1 };
			screenSettingScale = { 1.5f,1.5f };
			break;
		default:
			break;
		}
		break;
	case PadOperateSetting:
		switch (select)
		{
		case 0:
			selectFramePos = { 403,200 };
			break;
		case 1:
			selectFramePos = { 763,200 };
			break;
		case 2:
			selectFramePos = { 400,300 };
			break;
		case 3:
			selectFramePos = { 830,300 };
			break;
		case 4:
			selectFramePos = { 400,400 };
			break;
		case 5:
			selectFramePos = { 830,400 };
			break;
		default:
			break;
		}
		break;
	case PadSensitiveSetting:
		dotPos = { (480 - 228 * 1.5f) + 49 * 1.5f * (SettingParameters::padSensitivity - 1),300 - 20 * 1.5f };
		break;
	case ScreenSetting:
		switch (select)
		{
		case 0:
			selectFramePos = { 718,240 };
			break;
		case 1:
			selectFramePos = { 718,390 };
			break;
		default:
			break;
		}
		break;
	case EndCheck:
		switch (select)
		{
		case 0:
			yesScale = { 1.5f,1.5f };
			noScale = { 1,1 };
			break;
		case 1:
			noScale = { 1.5f,1.5f };
			yesScale = { 1,1 };
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
}

void Menu::Migrate()
{
	if (migrateCounter <= 30.0f)
	{
		texAlpha = Easing::EaseInOutQuartic(1.0f, 0.0f, 30.0f, (float)migrateCounter);
	}
	else if (migrateCounter <= 60.0f)
	{
		texAlpha = Easing::EaseInOutQuartic(0.0f, 1.0f, 30.0f, (float)migrateCounter - 30.0f);
	}
	else
	{
		migrate = false;
		migrated = false;
		migrateCounter = 0.0f;
		return;
	}
	if (migrateCounter >= 30.0f && !migrated)
	{
		menuState = nextMenuState;
		select = 0;
		migrated = true;
		SetTexParam();
	}
	migrateCounter += 60.0f * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);
}