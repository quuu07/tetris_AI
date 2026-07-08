#include "Game.h"
#include <cstring>
//bool headless
Game::Game()
    :headless(false), aiAssistEnabled(true), tTiles(), tBackground(), tButtons(), tSwitcher(), tFrame(), tCover(), tScore(), tGameOver(),
    sTiles(tTiles), sBackground(tBackground), sButtons(tButtons), sSwitcher(tSwitcher), sFrame(tFrame), sCover(tCover), sScore(tScore), sGameOver(tGameOver),
    font(), text(font),
    soundWin(sbWin), soundBoom(sbBoom)
{
    if (!headless) {
        Window_width = 1350;
        Window_height = 1000;
        ButtonState_Start = Start_Dark;
        imgSetNo = 1;
        uint8_t WindowStyle = sf::Style::Close | sf::Style::Titlebar;
        window.create(sf::VideoMode({ Window_width, Window_height }), L"Tetris by 李仕", WindowStyle);
        // 设置窗口位置等...
        //window.setFramerateLimit(15);  // 可以挪到 gameInitial 里，这里只是示例
    }


    //设置窗口在桌面上的位置	
    sf::VideoMode mode = sf::VideoMode::getDesktopMode();//查询桌面的属性
    Vector2i p = { 0,0 };
    p.x = (mode.size.x - Window_width) / 2;
    window.setPosition(p);
}

Game::~Game()
{
}

void Game::gameInitial()
{
    if (!headless) {
        window.setFramerateLimit(15);
    }
    LoadMediaData();

    isGameBegin = false;
    isGameHold = false;
    isGameOver = false;
    isGameQuit = false;
    //	ButtonState_Start = Start_Dark;
    ButtonState_Hold = Hold_Dark;
    player1.role = rolePLAYER1;		//定义Tetris对象为player1
    player2.role = rolePLAYER2;		//定义Tetris对象为player1
    player1.Initial(&tTiles, &window);		//将方块的素材传给Tetris对象，由Tetris对象绘制方块
    player2.Initial(&tTiles, &window);		//将方块的素材传给Tetris对象，由Tetris对象绘制方块
}
void Game::LoadMediaData()
{
    imgSetNo = 1;
    std::stringstream ss;
    ss << "./data/images/bg" << imgSetNo << ".jpg";//"data/images/BK01.png"

    if (!tBackground.loadFromFile(ss.str()))//加载纹理图片
    {
        std::cout << "BK image 没有找到" << std::endl;
    }
    ss.str("");//清空字符串
    ss << "./data/images/tiles" << imgSetNo << ".jpg";//"data/images/Game1.png"
    if (!tTiles.loadFromFile(ss.str()))
    {
        std::cout << "tiles.png 没有找到" << std::endl;
    }

    if (!tFrame.loadFromFile("./data/images/frame.png"))
    {
        std::cout << "frame.png 没有找到" << std::endl;
    }
    if (!tCover.loadFromFile("./data/images/cover.png"))
    {
        std::cout << "cover.png 没有找到" << std::endl;
    }
    if (!tGameOver.loadFromFile("./data/images/end.png"))
    {
        std::cout << "end.png 没有找到" << std::endl;
    }
    /*	if (!tScore.loadFromFile("data/images/recorder.jpg"))
        {
            std::cout << "recorder.jpg 没有找到" << std::endl;
        }*/
    if (!tButtons.loadFromFile("./data/images/button.png"))
    {
        std::cout << "button.png 没有找到" << std::endl;
    }
    if (!tSwitcher.loadFromFile("./data/images/bgSwitch.png"))
    {
        std::cout << "bgSwap.png 没有找到" << std::endl;
    }

    sBackground.setTexture(tBackground, true);					//设置精灵对象的纹理
    //sTiles.setTexture(tTiles);							//由Tetris对象绘制方块，两个玩家各自绘自己的方块
    sFrame.setTexture(tFrame, true);
    sCover.setTexture(tCover, true);
    sGameOver.setTexture(tGameOver, true);
    //sScore.setTexture(tScore);
    sButtons.setTexture(tButtons, true);
    sSwitcher.setTexture(tSwitcher, true);
    sSwitcher.setOrigin({ static_cast<float>(sSwitcher.getLocalBounds().size.x / 2.0),static_cast<float> (sSwitcher.getLocalBounds().size.y / 2.0) });

    if (!font.openFromFile(L"./data/Fonts/font.ttf"))//选择字体，SFML不能直接访问系统的字体，特殊的字体，需要自己加载
    {
        std::cout << "字体没有找到" << std::endl;
    }

}
void Game::TextOut()
{
    int initialX, initialY;
    int CharacterSize = 48;
    text.setCharacterSize(CharacterSize);
    text.setFillColor(Color(255, 0, 0, 255));//红色字体	
    text.setStyle(Text::Bold); // |Text::Underlined
    text.setPosition({ static_cast<float>(P1_SCORE_CORNER_X),static_cast<float> (P1_SCORE_CORNER_Y) });
    std::stringstream ss;
    //player1.score = 9999;
    ss << player1.score;
    text.setString(ss.str()); window.draw(text);

    text.setPosition({ static_cast<float>(P2_SCORE_CORNER_X - CharacterSize * 3), (P2_SCORE_CORNER_Y) });
    //player2.score = 6666;
    ss.str("");
    ss << player2.score;
    text.setString(ss.str()); window.draw(text);

    CharacterSize = 18;
    text.setCharacterSize(CharacterSize);
    text.setFillColor(Color(255, 255, 255, 255));
    text.setStyle(Text::Regular || Text::Italic);
    initialY = P1_STAGE_CORNER_Y + STAGE_HEIGHT * GRIDSIZE;
    text.setPosition({ static_cast<float>(P1_STAGE_CORNER_X), static_cast<float>(initialY) });
    text.setString(L"■ 感谢FamTrinl大佬提供的Tetris游戏基础代码"); window.draw(text);
    initialY += CharacterSize;
    text.setPosition({ static_cast<float>(P1_STAGE_CORNER_X), static_cast<float>(initialY) });
    text.setString(L"■ 感谢助教邓璇小姐姐提出的Tetris双打的建议    ■ 感谢助教丁筱琳小姐姐给做的游戏UI设计"); window.draw(text);
    initialY += CharacterSize;
    text.setPosition({ static_cast<float>(P1_STAGE_CORNER_X), static_cast<float>(initialY) });
    text.setString(L"■ 感谢金邵涵小哥哥教会我hard drop、底部缓冲、bag7、hold、wallkick、nextBlock等高端玩法"); window.draw(text);
    initialY += CharacterSize;
    text.setPosition({ static_cast<float>(P1_STAGE_CORNER_X), static_cast<float>(initialY) });
    text.setString(L"■ 感谢李文馨小姐姐让追加shadow效果以及消除动画效果的建议"); window.draw(text);
    initialY += CharacterSize;
    text.setPosition({ static_cast<float>(P1_STAGE_CORNER_X), static_cast<float>(initialY) });
    text.setString(L"■ 你们的建议和帮助，让这个版本的Tetris更加圆满！"); window.draw(text);
    initialY += CharacterSize;
    text.setPosition({ static_cast<float>(P1_STAGE_CORNER_X), static_cast<float>(initialY) });
    text.setString(L"■ 爱你们！！      李仕 写于20191114"); window.draw(text);

    CharacterSize = 24;
    text.setCharacterSize(CharacterSize);
    text.setFillColor(Color(255, 255, 255, 255));
    text.setStyle(Text::Bold); // |Text::Underlined
    initialY = INFO_CORNER_Y;
    text.setPosition({ static_cast<float>(INFO_CORNER_X), static_cast<float>(initialY) });
    text.setString(L"玩家1:"); window.draw(text);
    initialY += CharacterSize;
    text.setPosition({ static_cast<float>(INFO_CORNER_X), static_cast<float>(initialY) });
    text.setString(L"   方向键：WASD"); window.draw(text);
    initialY += CharacterSize;
    text.setPosition({ static_cast<float>(INFO_CORNER_X), static_cast<float>(initialY) });
    text.setString(L"   速降： Space键"); window.draw(text);
    initialY += CharacterSize;
    text.setPosition({ static_cast<float>(INFO_CORNER_X), static_cast<float>(initialY) });
    text.setString(L"   交换： 左Ctrl键"); window.draw(text);
    initialY += CharacterSize;
    text.setPosition({ static_cast<float>(INFO_CORNER_X), static_cast<float>(initialY) });
    text.setString(L"AI切换：Z"); window.draw(text);
    initialY += CharacterSize * 1.5;
    text.setPosition({ static_cast<float>(INFO_CORNER_X), static_cast<float>(initialY) });
    text.setString(L"玩家2:"); window.draw(text);
    initialY += CharacterSize;
    text.setPosition({ static_cast<float>(INFO_CORNER_X), static_cast<float>(initialY) });
    text.setString(L"   方向键：↑←↓→"); window.draw(text);
    initialY += CharacterSize;
    text.setPosition({ static_cast<float>(INFO_CORNER_X), static_cast<float>(initialY) });
    text.setString(L"   速降： Enter键"); window.draw(text);
    initialY += CharacterSize;
    text.setPosition({ static_cast<float>(INFO_CORNER_X), static_cast<float>(initialY) });
    text.setString(L"   交换： 右Ctrl键"); window.draw(text);
    initialY += CharacterSize;
    text.setPosition({ static_cast<float>(INFO_CORNER_X), static_cast<float>(initialY) });
    text.setString(L"AI切换： 0"); window.draw(text);
    initialY += 2 * CharacterSize;
    text.setPosition({ static_cast<float>(INFO_CORNER_X), static_cast<float>(initialY) });
    text.setString(L"退出： Esc键"); window.draw(text);
}
void Game::gameInput()
{

    window.setKeyRepeatEnabled(false);
    while (const std::optional event = window.pollEvent())
    {
        if (event->is<sf::Event::Closed>())
        {
            window.close();		//窗口可以移动、调整大小和最小化。但是如果要关闭，需要自己去调用close()函数
            isGameQuit = true;
        }
        if (const auto* KeyReleased = event->getIf<sf::Event::KeyReleased>())
        {
            if (KeyReleased->scancode == sf::Keyboard::Scancode::Escape)
            {
                window.close();  //窗口可以移动、调整大小和最小化。但是如果要关闭，需要自己去调用close()函数
                isGameQuit = true;
            }
        }

        if (const auto* mouseButtonReleased = event->getIf<sf::Event::MouseButtonReleased>())
        {
            if (mouseButtonReleased->button == sf::Mouse::Button::Left)
            {
                if (ButtonRectStart.contains({ mouseButtonReleased->position.x, mouseButtonReleased->position.y }))
                    if (isGameBegin == false)
                    {
                        isGameBegin = true;
                        player1.animationFlag = false;//初始化时候，动画的状态
                        player2.animationFlag = false;//初始化时候，动画的状态
                        ButtonState_Start = Close_Light;
                    }
                    else
                    {
                        isGameBegin = false;
                        ButtonState_Start = Start_Light;
                    }
                if (ButtonRectHold.contains({ mouseButtonReleased->position.x, mouseButtonReleased->position.y }))
                    if (isGameHold == false)
                    {
                        isGameHold = true;
                        ButtonState_Hold = Continue_Light;
                    }
                    else
                    {
                        isGameHold = false;
                        ButtonState_Hold = Hold_Light;
                    }

                if (ButtonRectAI.contains({ mouseButtonReleased->position.x, mouseButtonReleased->position.y }))
                {
                    aiAssistEnabled = !aiAssistEnabled;
                }
                if (ButtonRectLeft.contains({ mouseButtonReleased->position.x, mouseButtonReleased->position.y }))
                {
                    imgSetNo--;
                    if (imgSetNo < 1)//小于1的时候从皮肤4开始再循环
                        imgSetNo = 4;//重新轮换皮肤图
                    LoadMediaData();
                }
                if (ButtonRectRight.contains({ mouseButtonReleased->position.x, mouseButtonReleased->position.y }))
                {
                    imgSetNo++;
                    if (imgSetNo > 4)//大于皮肤图的总数时候
                        imgSetNo = 1;//重新轮换皮肤图
                    LoadMediaData();
                }
            }
        }
        if (const auto* mouseMoved = event->getIf<sf::Event::MouseMoved>())
        {
            if (ButtonRectStart.contains({ mouseMoved->position.x, mouseMoved->position.y }))
                if (isGameBegin == false)
                    ButtonState_Start = Start_Light;
                else
                    ButtonState_Start = Close_Light;
            else
                if (isGameBegin == false)
                    ButtonState_Start = Start_Dark;
                else
                    ButtonState_Start = Close_Dark;

            if (ButtonRectHold.contains({ mouseMoved->position.x, mouseMoved->position.y }))
                if (isGameHold == false)
                    ButtonState_Hold = Hold_Light;
                else
                    ButtonState_Hold = Continue_Light;
            else
                if (isGameHold == false)
                    ButtonState_Hold = Hold_Dark;
                else
                    ButtonState_Hold = Continue_Dark;
        }
        player1.Input(event);
        player2.Input(event);
    }

}
void Game::gameOverInput()
{
    window.setKeyRepeatEnabled(false);
    while (const std::optional event = window.pollEvent())
    {
        if (event->is<sf::Event::Closed>())
        {
            window.close();		//窗口可以移动、调整大小和最小化。但是如果要关闭，需要自己去调用close()函数
            isGameQuit = true;
        }
        if (const auto* KeyReleased = event->getIf<sf::Event::KeyReleased>())
        {
            if (KeyReleased->scancode == sf::Keyboard::Scancode::Escape)
            {
                window.close();  //窗口可以移动、调整大小和最小化。但是如果要关闭，需要自己去调用close()函数
                isGameQuit = true;
            }
        }
        if (const auto* mouseButtonReleased = event->getIf<sf::Event::MouseButtonReleased>())
        {
            if (mouseButtonReleased->button == sf::Mouse::Button::Left)
            {
                if (ButtonRectStart.contains({ mouseButtonReleased->position.x, mouseButtonReleased->position.y }))
                    // if (ButtonRectStart.contains(Mouse::getPosition(window)))
                {
                    isGameOver = false;
                }
                if (ButtonRectAI.contains({ mouseButtonReleased->position.x, mouseButtonReleased->position.y }))
                {
                    aiAssistEnabled = !aiAssistEnabled;
                }
            }
        }
        if (const auto* mouseMoved = event->getIf<sf::Event::MouseMoved>())
        {
            if (ButtonRectStart.contains({ mouseMoved->position.x, mouseMoved->position.y }))
                //    if (ButtonRectStart.contains(Mouse::getPosition(window)))
                ButtonState_Start = Close_Light;
            else
                ButtonState_Start = Close_Dark;
        }
    }
}
void Game::DrawButton()
{
    int ButtonWidth, ButtonHeight;

    ButtonWidth = 110;// sButtons.getLocalBounds().width / 8;//此处纹理有8个状态
    ButtonHeight = sButtons.getLocalBounds().size.y;
    //ButtonRectStart
    sButtons.setTextureRect(IntRect({ ButtonState_Start * ButtonWidth, 0 }, { ButtonWidth, ButtonHeight }));//读取按钮的纹理区域
    sButtons.setPosition({ static_cast<float>(B_START_CORNER_X), static_cast<float>(B_START_CORNER_Y) });								//设置按钮的位置坐标
    ButtonRectStart.position.x = B_START_CORNER_X;
    ButtonRectStart.position.y = B_START_CORNER_Y;
    ButtonRectStart.size.x = ButtonWidth;
    ButtonRectStart.size.y = ButtonHeight;
    window.draw(sButtons);
    //ButtonRectHold
    sButtons.setTextureRect(IntRect({ ButtonState_Hold * ButtonWidth, 0 }, { ButtonWidth, ButtonHeight }));//读取按钮的纹理区域
    sButtons.setPosition({ static_cast<float>(B_HOLD_CORNER_X), static_cast<float>(B_HOLD_CORNER_Y) });								//设置按钮的位置坐标
    ButtonRectHold.position.x = B_HOLD_CORNER_X;
    ButtonRectHold.position.y = B_HOLD_CORNER_Y;
    ButtonRectHold.size.x = ButtonWidth;
    ButtonRectHold.size.y = ButtonHeight;
    window.draw(sButtons);

    text.setFont(font);
    text.setCharacterSize(18);
    text.setStyle(sf::Text::Bold);
    text.setFillColor(sf::Color::White);
    text.setPosition({ static_cast<float>(P1_STAGE_CORNER_X + 10), 20.0f });
    text.setString(player1.aiEnabled ? L"玩家1 AI: 开" : L"玩家1 AI: 关");
    window.draw(text);
    text.setPosition({ static_cast<float>(P2_STAGE_CORNER_X + 10), 20.0f });
    text.setString(player2.aiEnabled ? L"玩家2 AI: 开" : L"玩家2 AI: 关");
    window.draw(text);

    // AI助手开关按钮
    sf::RectangleShape aiButtonShape({ 180.0f, 42.0f });
    aiButtonShape.setPosition({ static_cast<float>(B_AI_CORNER_X), static_cast<float>(B_AI_CORNER_Y) });
    aiButtonShape.setFillColor(aiAssistEnabled ? sf::Color(34, 139, 230, 220) : sf::Color(90, 90, 90, 220));
    aiButtonShape.setOutlineThickness(2.0f);
    aiButtonShape.setOutlineColor(sf::Color::White);
    window.draw(aiButtonShape);
    ButtonRectAI.position.x = B_AI_CORNER_X;
    ButtonRectAI.position.y = B_AI_CORNER_Y;
    ButtonRectAI.size.x = 180;
    ButtonRectAI.size.y = 42;
    text.setFont(font);
    text.setCharacterSize(22);
    text.setStyle(sf::Text::Bold);
    text.setFillColor(sf::Color::White);
    text.setPosition({ static_cast<float>(B_AI_CORNER_X + 18), static_cast<float>(B_AI_CORNER_Y + 8) });
    text.setString(aiAssistEnabled ? L"AI助手: 开" : L"AI助手: 关");
    window.draw(text);

    //背景素材切换
    ButtonWidth = sSwitcher.getLocalBounds().size.x;	//此处纹理就1个状态，大家可以参照前面的按钮自行添加
    ButtonHeight = sSwitcher.getLocalBounds().size.y;
    //ButtonRectLeft
    //sSwitcher.setTextureRect(IntRect(2 * ButtonWidth, 0, ButtonWidth, ButtonHeight));//读取按钮的纹理区域
    sSwitcher.setPosition({ static_cast<float>(B_LEFT_CORNER_X), static_cast<float>(B_LEFT_CORNER_Y) });								//设置按钮的位置坐标
    ButtonRectLeft.position.x = B_LEFT_CORNER_X - ButtonWidth / 2;
    ButtonRectLeft.position.y = B_LEFT_CORNER_Y - ButtonHeight / 2;
    ButtonRectLeft.size.x = ButtonWidth;
    ButtonRectLeft.size.y = ButtonHeight;
    window.draw(sSwitcher);
    //ButtonRectRight
    //sSwitcher.setTextureRect(IntRect(3 * ButtonWidth, 0, ButtonWidth, ButtonHeight));//读取按钮的纹理区域
    sSwitcher.setPosition({ static_cast<float>(B_RIGHT_CORNER_X), static_cast<float>(B_RIGHT_CORNER_Y) });								//设置按钮的位置坐标
    ButtonRectRight.position.x = B_RIGHT_CORNER_X - ButtonWidth / 2;
    ButtonRectRight.position.y = B_RIGHT_CORNER_Y - ButtonHeight / 2;
    ButtonRectRight.size.x = ButtonWidth;
    ButtonRectRight.size.y = ButtonHeight;
    sSwitcher.rotate(sf::degrees(180));//只有一个箭头素材，当然要靠自己代码生成另一个^_^
    window.draw(sSwitcher);
    sSwitcher.rotate(sf::degrees(180));//还原角度

}
void Game::DrawResults()
{
    // 如果游戏没有结束，直接返回
    if (!isGameOver) return;

    int ButtonWidth = 250;
    int ButtonHeight = sGameOver.getLocalBounds().size.y;

    // 决定谁赢了：分数高者赢，平局则同时结束？
    bool player1Wins = (player1.score > player2.score);
    bool player2Wins = (player2.score > player1.score);

    // 显示玩家1的结果
    sGameOver.setTextureRect(IntRect({ player1Wins ? 0 : ButtonWidth, 0 }, { ButtonWidth, ButtonHeight }));
    sGameOver.setPosition({ static_cast<float>(P1_STAGE_CORNER_X + GRIDSIZE * 1.5), 0.0 });
    window.draw(sGameOver);

    // 显示玩家2的结果
    sGameOver.setTextureRect(IntRect({ player2Wins ? 0 : ButtonWidth, 0 }, { ButtonWidth, ButtonHeight }));
    sGameOver.setPosition({ static_cast<float>(P2_STAGE_CORNER_X + GRIDSIZE * 1.5), 0.0 });
    window.draw(sGameOver);
}
//ga
float Game::simulateOneGame(float weights[6]) {
    std::cout << "Simulation started" << std::endl;
    gameInitial();
    memcpy(player2.pdWeights, weights, 6 * sizeof(float));
    isGameBegin = true;
    isGameHold = false;

    // 强制关闭动画，避免阻塞
    player1.animationFlag = false;
    player2.animationFlag = false;

    int frameCount = 0;
    while (!player2.isGameOver) {
        frameCount++;

        // 手动推进 timer
        player1.timer += 0.05f;
        player2.timer += 0.05f;

        // AI 决策
        player2.AIAutoPlay();

        // 执行逻辑
        player1.Logic();
        player2.Logic();

        // 检查游戏结束
        isGameOver = (player1.isGameOver || player2.isGameOver);

        // 每100帧输出一次状态
        if (frameCount % 100 == 0) {
            int blockCount = 0;
            for (int i = 0; i < FIELD_HEIGHT; i++)
                for (int j = 0; j < FIELD_WIDTH; j++)
                    if (player2.Field[i][j] != 0) blockCount++;
            std::cout << "Frame " << frameCount << ", isGameOver=" << player2.isGameOver 
                      << ", blocks=" << blockCount << std::endl;
        }

        // 如果方块数量不再增加且 gameover 仍为 false，可能卡住，提前退出
        if (frameCount > 5000) {
            std::cout << "Frame limit reached, forcing exit" << std::endl;
            break;
        }
    }

    std::cout << "Simulation ended, score=" << player2.score << std::endl;
    return player2.score;
}
//ga

void Game::gameLogic()
{
    if (isGameHold == true)
        return;
    float time = clock.getElapsedTime().asSeconds();
    clock.restart();

    bool p1Lost = player1.isGameOver;
    bool p2Lost = player2.isGameOver;

    if (p1Lost && p2Lost)
    {
        isGameOver = true;
        return;
    }

    if (!p1Lost)
        player1.timer += time;
    if (!p2Lost)
        player2.timer += time;

    if (!p1Lost && player1.aiEnabled)
        player1.AIAutoPlay();
    if (!p2Lost && player2.aiEnabled)
        player2.AIAutoPlay();

    if (!p1Lost)
        player1.Logic();
    if (!p2Lost)
        player2.Logic();

    isGameOver = (player1.isGameOver && player2.isGameOver);
}
void Game::gameDraw()
{
    window.clear();	//清屏
    //绘制背景
    sBackground.setPosition({ 0.0, 0.0 });
    window.draw(sBackground);
    window.draw(sFrame);

    player1.Draw();
    player2.Draw();

    sCover.setPosition({ static_cast<float>(P1_STAGE_CORNER_X),  static_cast<float>(P1_STAGE_CORNER_Y) });
    window.draw(sCover);
    sCover.setPosition({ static_cast<float>(P2_STAGE_CORNER_X),  static_cast<float>(P2_STAGE_CORNER_Y) });
    window.draw(sCover);

    DrawButton();
    TextOut();

    bool p1Lost = player1.isGameOver;
    bool p2Lost = player2.isGameOver;
    if ((p1Lost && !p2Lost) || (!p1Lost && p2Lost))
    {
        if (p1Lost)
        {
            // 只遮挡玩家1的游戏区域
            sf::RectangleShape overlay({ static_cast<float>(STAGE_WIDTH * GRIDSIZE), static_cast<float>(STAGE_HEIGHT * GRIDSIZE) });
            overlay.setPosition({ static_cast<float>(P1_STAGE_CORNER_X), static_cast<float>(P1_STAGE_CORNER_Y) });
            overlay.setFillColor(sf::Color(0, 0, 0, 200));
            window.draw(overlay);

            text.setFont(font);
            text.setCharacterSize(32);
            text.setStyle(sf::Text::Bold);
            text.setFillColor(sf::Color::White);
            text.setPosition({ static_cast<float>(P1_STAGE_CORNER_X + 20), static_cast<float>(P1_STAGE_CORNER_Y + 140) });
            text.setString(L"玩家1失败");
            window.draw(text);

            text.setCharacterSize(24);
            text.setPosition({ static_cast<float>(P1_STAGE_CORNER_X + 30), static_cast<float>(P1_STAGE_CORNER_Y + 190) });
            text.setString(L"玩家2继续");
            window.draw(text);

            text.setCharacterSize(18);
            text.setPosition({ static_cast<float>(P1_STAGE_CORNER_X + 20), static_cast<float>(P1_STAGE_CORNER_Y + 280) });
            text.setString(L"输赢会在双方都");
            window.draw(text);

            text.setPosition({ static_cast<float>(P1_STAGE_CORNER_X + 20), static_cast<float>(P1_STAGE_CORNER_Y + 310) });
            text.setString(L"失败后显示");
            window.draw(text);
        }
        else
        {
            // 只遮挡玩家2的游戏区域
            sf::RectangleShape overlay({ static_cast<float>(STAGE_WIDTH * GRIDSIZE), static_cast<float>(STAGE_HEIGHT * GRIDSIZE) });
            overlay.setPosition({ static_cast<float>(P2_STAGE_CORNER_X), static_cast<float>(P2_STAGE_CORNER_Y) });
            overlay.setFillColor(sf::Color(0, 0, 0, 200));
            window.draw(overlay);

            text.setFont(font);
            text.setCharacterSize(32);
            text.setStyle(sf::Text::Bold);
            text.setFillColor(sf::Color::White);
            text.setPosition({ static_cast<float>(P2_STAGE_CORNER_X + 20), static_cast<float>(P2_STAGE_CORNER_Y + 140) });
            text.setString(L"玩家2失败");
            window.draw(text);

            text.setCharacterSize(24);
            text.setPosition({ static_cast<float>(P2_STAGE_CORNER_X + 30), static_cast<float>(P2_STAGE_CORNER_Y + 190) });
            text.setString(L"玩家1继续");
            window.draw(text);

            text.setCharacterSize(18);
            text.setPosition({ static_cast<float>(P2_STAGE_CORNER_X + 20), static_cast<float>(P2_STAGE_CORNER_Y + 280) });
            text.setString(L"比分会在双方都");
            window.draw(text);

            text.setPosition({ static_cast<float>(P2_STAGE_CORNER_X + 20), static_cast<float>(P2_STAGE_CORNER_Y + 310) });
            text.setString(L"失败后显示");
            window.draw(text);
        }
    }

    DrawResults();

    window.display();				//把显示缓冲区的内容，显示在屏幕上。SFML采用的是双缓冲机制
}

void Game::gameRun()
{
    do {

        gameInitial();

        while (window.isOpen() && isGameOver == false)
        {
            gameInput();

            if (isGameBegin == true)
                gameLogic();
            else
                gameInitial();

            gameDraw();
        }
        while (window.isOpen() && isGameOver == true)
        {
            gameOverInput();
            gameDraw();
        }
    } while (!isGameQuit);
}