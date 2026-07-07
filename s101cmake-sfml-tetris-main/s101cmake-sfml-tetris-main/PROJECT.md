# Tetris 双打项目 - AI 上下文文档

## 1. 项目概览
- 基于 **SFML** 的 C++ 双人俄罗斯方块。
- 双玩家同屏竞技，独立计分与消行。

## 2. 架构分层
- **Game 类**：管理窗口（`sf::RenderWindow`）、事件轮询、纹理加载、按钮（开始/暂停/换肤），持有 `player1` 和 `player2` 两个 `Tetris` 实例。
- **Tetris 类**：每个玩家的独立逻辑实例。包含 `Field[FIELD_HEIGHT][FIELD_WIDTH]`、当前活动方块 `currentSquare[4]`、Next 和 Hold 方块，以及碰撞、旋转、消行等函数。

## 3. 关键常量和坐标（修改时必须遵循）
- `GRIDSIZE = 35`（像素）。
- 舞台尺寸：`FIELD_WIDTH = 10`，`FIELD_HEIGHT = 24`（顶部 4 行为隐藏生成区）。
- 玩家 1 舞台绘制起点：`P1_STAGE_CORNER_X = 156`，`P1_STAGE_CORNER_Y = 174`。
- 玩家 2 舞台绘制起点：`P2_STAGE_CORNER_X = 844`，`P2_STAGE_CORNER_Y = 174`。
- 玩家 1 得分显示：`P1_SCORE_CORNER_X = 366`，`P1_SCORE_CORNER_Y = 110`。
- 玩家 2 得分显示：`P2_SCORE_CORNER_X = 1195`，`P2_SCORE_CORNER_Y = 110`。
- Hold 区显示：`HOLD_CORNER_X = 660`，`HOLD_CORNER_Y = 595`。
- Next 区显示：`P1_NXET_CORNER_X = 587`，`P1_NXET_CORNER_Y = 125`（P2 对称）。

## 4. 输入映射（严格区分）
- **玩家 1**：移动（W/A/S/D），旋转（W），硬降（Space），Hold 交换（Left Control），加速下落（按住 S）。
- **玩家 2**：移动（↑/←/↓/→），旋转（↑），硬降（Enter），Hold 交换（Right Control），加速下落（按住 ↓）。
- 退出：`Esc`。

## 5. 核心游戏机制（AI 生成功能时必须保留）
- **Bag7 随机器**：`Tetris::Bag7()` 保证 7 个方块全部出现一次后才重复。
- **Wall Kick（墙踢）**：旋转时中心点偏移尝试修正，代码实现在 `rotateFunc()` 中。
- **底部缓冲（Soft Lock）**：当方块接近底部时，`slowLoading()` 会让下落速度暂时减半（`delay = DELAYVALUE * 2`）。
- **Shadow（阴影）**：`shadow()` 计算当前方块落点，以灰色半透明绘制。
- **Hold 交换**：支持交换当前方块与 Hold 区方块，需处理初始为空的情况。
- **消行动画**：消行时触发 `animationFlag = true`，方块旋转缩放消失，完成后才调用 `clearLine()`。

## 6. 主循环流程（AI 添加功能时请勿破坏顺序）
`Game::gameRun()` → 初始加载 → `while(window.isOpen() && !isGameOver)`：
1. `gameInput()`（事件轮询，更新 `dx`、`isRotate` 等标志）。
2. `gameLogic()`（调用 `player.Logic()`，内部处理下落、碰撞、消行检测）。
3. `gameDraw()`（渲染背景、方块、按钮、文字）。

## 7. AI 协作规则（重要）
- **修改坐标前**：先确认使用的是 `P1_`/`P2_` 宏，不要硬编码数字。
- **修改逻辑前**：保留 `animationFlag` 和 `newShapeFlag` 的协作机制，新增功能需在 `traditonLogic()` 或 `Logic()` 中显式调用。
- **添加新方块形状**：需同步修改 `Figures` 数组和 `Bag7()`，并确保枚举 `gridShape` 顺序一致。
- **音效/资源加载**：所有纹理和字体统一在 `Game::LoadMediaData()` 中初始化，新增资源请参照该函数格式。
- **代码风格**：保持使用 `sf::` 类型（如 `Vector2i`、`IntRect`），事件处理用 `std::optional<sf::Event>`。

## 8. 当前已知的架构隐患（提醒 AI 不要踩坑）
- `Tetris.h` 中的 `static int holdcolorNum, holdShapeNum;` 和 `static Vector2i holdSquare[4];` 是**全局共享**的，会导致玩家 1 和玩家 2 的 Hold 互相干扰。如要修复，应改为 `Tetris` 的成员变量。