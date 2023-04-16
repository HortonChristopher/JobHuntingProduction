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

	if (!Sprite::LoadTexture(401, "menuUI.png")) { assert(0); return; }
	if (!Sprite::LoadTexture(402, "setting.png")) { assert(0); return; }
	if (!Sprite::LoadTexture(403, "end.png")) { assert(0); return; }
	if (!Sprite::LoadTexture(404, "sensitiveSetting.png")) { assert(0); return; }
	if (!Sprite::LoadTexture(405, "operateSetting.png")) { assert(0); return; }
	if (!Sprite::LoadTexture(406, "ScreenSetting.png")) { assert(0); return; }
	if (!Sprite::LoadTexture(407, "upsideDown.png")) { assert(0); return; }
	if (!Sprite::LoadTexture(408, "check.png")) { assert(0); return; }
	if (!Sprite::LoadTexture(409, "flipLeftRight.png")) { assert(0); return; }
	if (!Sprite::LoadTexture(410, "a.png")) { assert(0); return; }
	if (!Sprite::LoadTexture(411, "cameraReset.png")) { assert(0); return; }
	if (!Sprite::LoadTexture(412, "action.png")) { assert(0); return; }
	if (!Sprite::LoadTexture(413, "changeRun.png")) { assert(0); return; }
	if (!Sprite::LoadTexture(414, "selectFrame.png")) { assert(0); return; }
	if (!Sprite::LoadTexture(415, "dot.png")) { assert(0); return; }
	if (!Sprite::LoadTexture(416, "sensitiveSettingBar.png")) { assert(0); return; }
	if (!Sprite::LoadTexture(417, "ScreenSetting.png")) { assert(0); return; }
	if (!Sprite::LoadTexture(418, "viewCollision.png")) { assert(0); return; }
	if (!Sprite::LoadTexture(419, "sensitiveSettingBar.png")) { assert(0); return; }
	if (!Sprite::LoadTexture(420, "endCheck.png")) { assert(0); return; }
	if (!Sprite::LoadTexture(421, "yes.png")) { assert(0); return; }
	if (!Sprite::LoadTexture(422, "no.png")) { assert(0); return; }
	if (!Sprite::LoadTexture(423, "b.png")) { assert(0); return; }
	if (!Sprite::LoadTexture(424, "x.png")) { assert(0); return; }
	if (!Sprite::LoadTexture(425, "y.png")) { assert(0); return; }
	if (!Sprite::LoadTexture(426, "LT.png")) { assert(0); return; }
	if (!Sprite::LoadTexture(427, "RT.png")) { assert(0); return; }
	if (!Sprite::LoadTexture(428, "LB.png")) { assert(0); return; }
	if (!Sprite::LoadTexture(429, "RB.png")) { assert(0); return; }
	if (!Sprite::LoadTexture(430, "controllerOperation.png")) { assert(0); return; }
	if (!Sprite::LoadTexture(431, "keyboardOperation.png")) { assert(0); return; }

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
	dotPos = { 380.0f, 436.0f };
	selectFramePos = { 370.0f, 207.0f };
	operateChangeFlag = false;
	returnTitle = false;
	texAlpha = 1;
	migrateCounter = 0;
	migrate = false;
	migrated = false;

	menuUI = Sprite::Create(401, { 240.0f, 110.0f });
	menuUI->SetSize({ 800.0f, 500.0f });
	setting = Sprite::Create(402, { 380.0f, 217.0f });
	end = Sprite::Create(403, { 380.0f, 436.0f });
	sensitivitySetting = Sprite::Create(404, { 380.0f, 217.0f });
	operateSetting = Sprite::Create(405, { 380.0f, 326.5f });
	screenSetting = Sprite::Create(406, { 380.0f, 436.0f });
	upsideDown = Sprite::Create(407, { 295.0f, 200.0f });
	check = Sprite::Create(408, { 403.0f, 196.0f });
	flipLeftRight = Sprite::Create(409, { 655.0f, 200.0f });
	check2 = Sprite::Create(408, { 720.0f, 390.0f });
	cameraReset = Sprite::Create(411, { 620.0f, 300.0f });
	action = Sprite::Create(412, { 240.0f, 400.0f });
	changeRun = Sprite::Create(413, { 620.0f, 400.0f });
	selectFrame = Sprite::Create(414, selectFramePos);
	selectFrame->SetSize({ 148.0f, 87.0f });
	dot = Sprite::Create(415, dotPos);
	dot->SetSize({ 27.0f, 27.0f });
	sensitiveSettingBar = Sprite::Create(416, { 380.0f, 436.0f });
	viewCollision = Sprite::Create(418, { 380.0f, 436.0f });
	endCheck = Sprite::Create(420, { 300.0f, 180.0f });
	yes = Sprite::Create(421, { 380.0f, 380.0f });
	no = Sprite::Create(422, { 710.0f, 380.0f });
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
		setting->SetPosition({ 380.0f, 217.0f });
		setting->SetColor({ 1.0f, 1.0f, 1.0f, texAlpha });
		setting->Draw();
		end->SetPosition({ 380.0f, 436.0f });
		end->SetColor({ 1.0f, 1.0f, 1.0f, texAlpha });
		end->Draw();
		selectFrame->SetPosition(selectFramePos);
		selectFrame->SetColor({ 1.0f, 1.0f, 1.0f, texAlpha });
		selectFrame->Draw();
		break;

	case SettingSelect:
		sensitivitySetting->SetPosition({ 380.0f, 217.0f });
		sensitivitySetting->SetColor({ 1.0f, 1.0f, 1.0f, texAlpha });
		sensitivitySetting->Draw();
		operateSetting->SetPosition({ 380.0f, 326.5f });
		operateSetting->SetColor({ 1.0f, 1.0f, 1.0f, texAlpha });
		operateSetting->Draw();
		/*screenSetting->SetPosition({ 380.0f, 436.0f });
		screenSetting->SetColor({ 1.0f, 1.0f, 1.0f, texAlpha });
		screenSetting->Draw();*/
		selectFrame->SetPosition(selectFramePos);
		selectFrame->SetColor({ 1.0f, 1.0f, 1.0f, texAlpha });
		selectFrame->Draw();
		break;

	case PadOperateSetting:
		operateSetting->SetPosition({ 380.0f, 217.0f });
		operateSetting->SetColor({ 1.0f, 1.0f, 1.0f, texAlpha });
		operateSetting->Draw();

		//upsideDown->SetPosition({ 280.0f, 326.5f });
		//upsideDown->SetColor({ 1.0f, 1.0f, 1.0f, texAlpha });
		//upsideDown->Draw();
		////ONの時のみチェックマークを描画
		//if (SettingParameters::reverseY == -1)
		//{
		//	check->SetPosition({ 480.0f, 323.5f });
		//	check->SetColor({ 1.0f, 1.0f, 1.0f, texAlpha });
		//	check->Draw();
		//}

		flipLeftRight->SetPosition({ 280.0f, 326.5f }); // normally { 580.0f, 326.5f }
		flipLeftRight->SetColor({ 1.0f, 1.0f, 1.0f, texAlpha });
		flipLeftRight->Draw();

		//ONの時のみチェックマークを描画
		if (SettingParameters::reverseX == -1)
		{
			check2->SetPosition({ 480.0f, 308.5f }); // normally { 773.0f, 320.5f }
			check2->SetColor({ 1.0f, 1.0f, 1.0f, texAlpha });
			check2->Draw();
		}

		//cameraReset->Draw();
		//cameraReset->SetColor({ 1.0f, 1.0f, 1.0f, texAlpha });
		//cameraResetOperate->DrawSprite(operateTexNames[SettingParam::resetButton], { 830,300 }, 0, { 1,1 }, { 1,1,1,texAlpha });
		//action->Draw();
		//action->SetColor({ 1.0f, 1.0f, 1.0f, texAlpha });
		//actionOperate->DrawSprite(operateTexNames[SettingParam::airSlideButton], { 400,400 }, 0, { 1,1 }, { 1,1,1,texAlpha });
		//changeRun->Draw();
		//changeRun->SetColor({ 1.0f, 1.0f, 1.0f, texAlpha });
		//changeRunOperate->DrawSprite(operateTexNames[SettingParam::runButton], { 830,400 }, 0, { 1,1 }, { 1,1,1,texAlpha });
		/*selectFrame->SetPosition(selectFramePos);
		selectFrame->SetColor({ 1.0f, 1.0f, 1.0f, texAlpha });
		selectFrame->Draw();*/
		break;

	case PadSensitiveSetting:
		sensitivitySetting->SetColor({ 1.0f, 1.0f, 1.0f, texAlpha });
		sensitivitySetting->Draw();
		dot->SetPosition(dotPos);
		dot->SetSize({ 27.0f, 27.0f });
		dot->SetColor({ 1.0f, 1.0f, 1.0f, texAlpha });
		dot->Draw();
		sensitiveSettingBar->SetColor({ 1.0f, 1.0f, 1.0f, texAlpha });
		sensitiveSettingBar->Draw();
		break;

	case ScreenSetting:
		screenSetting->SetPosition({ 380.0f, 217.0f });
		screenSetting->SetColor({ 1.0f, 1.0f, 1.0f, texAlpha });
		screenSetting->Draw();
		viewCollision->SetPosition({ 380.0f, 436.0f });
		viewCollision->SetColor({ 1.0f, 1.0f, 1.0f, texAlpha });
		viewCollision->Draw();

		//ONの時のみチェックマークを描画
		if (SettingParameters::viewCollision)
		{
			check->SetPosition({ 845.0f, 436.0f });
			check->SetColor({ 1.0f, 1.0f, 1.0f, texAlpha });
			check->Draw();
		}
		break;

	case EndCheck:
		endCheck->SetPosition({ 300.0f, 180.0f });
		endCheck->SetColor({ 1.0f, 1.0f, 1.0f, texAlpha });
		endCheck->Draw();
		yes->SetPosition({ 380.0f, 380.0f });
		yes->SetColor({ 1.0f, 1.0f, 1.0f, texAlpha });
		yes->Draw();
		no->SetPosition({ 710.0f, 380.0f });
		no->SetColor({ 1.0f, 1.0f, 1.0f, texAlpha });
		no->Draw();
		selectFrame->SetPosition(selectFramePos);
		selectFrame->SetColor({ 1.0f, 1.0f, 1.0f, texAlpha });
		selectFrame->Draw();
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

	if (input->TriggerControllerButton(XINPUT_GAMEPAD_A) || input->TriggerKey(DIK_SPACE))
	{
		switch (select)
		{
		case 0:
			nextMenuState = SettingSelect;
			break;
		case 1:
			nextMenuState = EndCheck;
			break;
		default:
			return;
		}
		migrate = true;
		SetTexParam();
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
			if (select < 1) // Normally is 2
				select++;
		}
		if (input->TriggerLStickUp() || input->TriggerControllerButton(XINPUT_GAMEPAD_DPAD_UP) || input->TriggerKey(DIK_W))
		{
			if (select > 0)
				select--;
		}
		SetTexParam();
	}

 	if (input->TriggerControllerButton(XINPUT_GAMEPAD_A) || input->TriggerKey(DIK_SPACE))
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

	if (input->TriggerControllerButton(XINPUT_GAMEPAD_B) || input->TriggerKey(DIK_X))
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

	if (input->TriggerControllerButton(XINPUT_GAMEPAD_B) || input->TriggerKey(DIK_X))
	{
		nextMenuState = SettingSelect;
		migrate = true;
		SetTexParam();
	}
}

void Menu::OperateSetting()
{
	if (input->TriggerLStickDown() || input->TriggerControllerButton(XINPUT_GAMEPAD_DPAD_DOWN)
		|| input->TriggerLStickUp() || input->TriggerControllerButton(XINPUT_GAMEPAD_DPAD_UP) ||
		input->TriggerLStickLeft() || input->TriggerControllerButton(XINPUT_GAMEPAD_DPAD_LEFT)
		|| input->TriggerLStickRight() || input->TriggerControllerButton(XINPUT_GAMEPAD_DPAD_RIGHT)
		|| input->TriggerKey(DIK_A) || input->TriggerKey(DIK_D) || input->TriggerKey(DIK_W) || input->TriggerKey(DIK_S))
	{
		//if (input->TriggerLStickRight() || input->TriggerControllerButton(XINPUT_GAMEPAD_DPAD_RIGHT) || input->TriggerKey(DIK_D))
		//{
		//	if (select < 5) // Default 5
		//		select++;
		//}
		//if (input->TriggerLStickLeft() || input->TriggerControllerButton(XINPUT_GAMEPAD_DPAD_LEFT) || input->TriggerKey(DIK_A))
		//{
		//	if (select > 0)
		//		select--;
		//}
		/*if (input->TriggerLStickDown() || input->TriggerControllerButton(XINPUT_GAMEPAD_DPAD_DOWN) || input->TriggerKey(DIK_S))
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
		}*/
		SetTexParam();
	}
	if (input->TriggerControllerButton(XINPUT_GAMEPAD_A) || input->TriggerKey(DIK_SPACE))
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

	if (input->TriggerControllerButton(XINPUT_GAMEPAD_B) || input->TriggerKey(DIK_X))
	{
		nextMenuState = SettingSelect;
		migrate = true;
		SetTexParam();
	}
}

void Menu::ControllerButton()
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
}

void Menu::KeyboardKey()
{

}

void Menu::ScreenSettingProcess()
{
	if (input->TriggerControllerButton(XINPUT_GAMEPAD_A) || input->TriggerKey(DIK_SPACE))
	{
		switch (select)
		{
		case 0:
			SettingParameters::viewCollision = SettingParameters::viewCollision == true ? false : true;
			break;
		default:
			return;
		}
		SetTexParam();
	}

	if (input->TriggerControllerButton(XINPUT_GAMEPAD_B) || input->TriggerKey(DIK_X))
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

	if (input->TriggerControllerButton(XINPUT_GAMEPAD_A) || input->TriggerKey(DIK_SPACE))
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

	if (input->TriggerControllerButton(XINPUT_GAMEPAD_B) || input->TriggerKey(DIK_X))
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
			selectFramePos = { 370.0f, 207.0f };
			selectFrame->SetPosition(selectFramePos);
			selectFrame->SetSize({ 148.0f, 87.0f });
			break;
		case 1:
			restartScale = { 1.5f,1.5f };
			endScale = { 1,1 };
			settingScale = { 1,1 };
			selectFramePos = { 370.0f, 426.0f };
			selectFrame->SetPosition(selectFramePos);
			selectFrame->SetSize({ 458.0f, 87.0f });
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
			selectFramePos = { 370.0f, 207.0f };
			selectFrame->SetPosition(selectFramePos);
			selectFrame->SetSize({ 272.0f, 87.0f });
			break;
		case 1:
			operateSettingScale = { 1.5f,1.5f };
			sensitiveSettingScale = { 1,1 };
			screenSettingScale = { 1,1 };
			selectFramePos = { 370.0f, 316.5f };
			selectFrame->SetPosition(selectFramePos);
			selectFrame->SetSize({ 272.0f, 87.0f });
			break;
		case 2:
			operateSettingScale = { 1,1 };
			sensitiveSettingScale = { 1,1 };
			screenSettingScale = { 1.5f,1.5f };
			selectFramePos = { 370.0f, 426.0f };
			selectFrame->SetPosition(selectFramePos);
			selectFrame->SetSize({ 278.0f, 87.0f });
			break;
		default:
			break;
		}
		break;
	case PadOperateSetting:
		switch (select)
		{
		case 0:
			selectFramePos = { 270.0f, 316.5f };
			selectFrame->SetPosition(selectFramePos);
			selectFrame->SetSize({ 285.0f, 73.0f });
			break;
		case 1:
			selectFramePos = { 570.0f, 316.5f };
			selectFrame->SetPosition(selectFramePos);
			selectFrame->SetSize({ 285.0f, 73.0f });
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
		dotPos = { 380.0f + 49.0f * (SettingParameters::padSensitivity - 1), 446.0f };
		break;
	case ScreenSetting:
		break;
	case EndCheck:
		switch (select)
		{
		case 0:
			selectFramePos = { 370.0f, 370.0f };
			selectFrame->SetPosition(selectFramePos);
			selectFrame->SetSize({ 148.0f, 87.0f });
			yesScale = { 1.5f,1.5f };
			noScale = { 1,1 };
			break;
		case 1:
			selectFramePos = { 700.0f, 370.0f };
			selectFrame->SetPosition(selectFramePos);
			selectFrame->SetSize({ 210.0f, 87.0f });
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
		if (menuState == PadOperateSetting)
		{
			select = 1;
		}
		else
		{
			select = 0;
		}
		migrated = true;
		SetTexParam();
	}
	migrateCounter += 60.0f * (deltaTime->deltaTimeCalculated.count() / 1000000.0f);
}