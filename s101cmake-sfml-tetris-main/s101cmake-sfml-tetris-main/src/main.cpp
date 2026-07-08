/*
 * 项目名称：s101cmake-sfml-tetris
 * 开发者：李仕
 * 所属组织：xLab-HDU
 * 代码仓库：https://github.com/xLab-HDU
 * 
 * 本代码为开源项目，欢迎提交 Issue 和 Pull Request。
 */

#include "Game.h"
#include "GeneticAlgorithm.h"

int main(int argc, char* argv[])
{
    if (argc > 1 && std::string(argv[1]) == "--ga") {
        runGA();   // 运行遗传算法
        return 0;
    }
	
	Game tetrisGame;
	while (tetrisGame.window.isOpen())
	{
		tetrisGame.gameRun();
	}
	//runGA();   // 运行遗传算法
	return 0;
}