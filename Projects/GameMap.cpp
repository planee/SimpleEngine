#include "GameMap.h"
#include "Astar.h"
#include <SOIL/SOIL.h>
#include <random>
#include <iostream>

GameMap::GameMap(std::string filename)
{
	mXyqMap = new NetEase::MAP(filename);

	//地图读取后的宽高
	mMapWidth = mXyqMap->m_MapWidth;
	mMapHeight = mXyqMap->m_MapHeight;

	//地图实际宽高
	mWidth = mXyqMap->m_Width;
	mHeight = mXyqMap->m_Height;

	//地图的行列数
	mRow = mXyqMap->m_RowCount;
	mCol = mXyqMap->m_ColCount;
	printf("初始化GameMap %d %d ", mRow, mCol);

	mMapTiles.clear();
	for (int i = 0; i<mXyqMap->m_UnitSize; i++)
	{
		mXyqMap->ReadUnit(i);
		
		mMapTiles.push_back(new Texture(320,240, false,(uint8*)(mXyqMap->m_MapUnits)[i].BitmapRGB24) );
		// delete[] mXyqMap->m_MapUnits[i].BitmapRGB24;
		mXyqMap->m_MapUnits[i].BitmapRGB24 = nullptr;
	}

	mMaskTiles.clear();
	for (int i = 0; i<mXyqMap->m_MaskSize; i++)
	{
		mXyqMap->ReadMask(i);


		mMaskTiles.push_back(new Texture(mXyqMap->m_MaskInfos[i].Width,
			mXyqMap->m_MaskInfos[i].Height,true, (uint8*)(mXyqMap->m_MaskInfos)[i].Data ));

		// delete[] mXyqMap->m_MaskInfos[i].Data;
		mXyqMap->m_MaskInfos[i].Data = nullptr;
	}

	delete[] mXyqMap->m_MapPixelsRGB24;
	mXyqMap->m_MapPixelsRGB24 = nullptr;
	
	mCellWidth = mCol * 16;
	mCellHeight = mRow * 12;
	mCell = new int*[mCellWidth];
	for (int i = 0; i < mCellWidth; i++)
	{
		mCell[i] = new int[mCellHeight];
	}

	int x = 0, y = 0;
	for (int i = 0; i < mRow; i++)
	{
		for (int j = 0; j < mCol; j++)
		{
			y = 12 * i;	//确定y的开始坐标
			for (int p = 0; p < 12; p++)
			{
				x = 16 * j;	//确定x的开始坐标
				for (int q = 0; q < 16; q++)
				{
					mCell[x++][y] = (mXyqMap->m_MapUnits)[i*mCol + j].Cell[p * 16 + q];
				}
				y++;
			}
		}
	}
	mAstar = new Astar(mCellWidth, mCellHeight, mCell);

	// std::ofstream outfile("a.map");
	// for (int y = 0; y < mCellHeight; y++) 
	// {
	// 	for (int x = 0; x < mCellWidth; x++) {
	// 		//printf("%d", mMap[x][y]);
	// 		outfile << mCell[x][y];
	// 	}
	// 	outfile << '\n';
	// 	//printf("\n");
	// }
	// outfile.close();

	/*初始化障碍图片*/
	// int width, height;
	// unsigned char* image = SOIL_load_image("X.png", &width, &height, 0,
	// 	SOIL_LOAD_RGBA);
	
	// mCellPic = Texture(image,true);
	// SOIL_free_image_data(image);

}

void GameMap::clamp(int val, int min, int max)
{
	if (val < min) val = min;
	if (val > max) val = max;
}

NetEase::MAP* GameMap::GetMapPtr()
{
	return mXyqMap;
}

std::list<Pos> GameMap::Move(int sx, int sy, int ex, int ey)
{
	//printf(" GameMap:sx %d sy %d ex %d ey %d\n", sx, sy, ex, ey);
	if (sx < 0 || sy < 0 || ex < 0 || ey < 0) return{};
	if (sx > mCellWidth || ex > mCellWidth || sy > mCellHeight || ey > mCellHeight) return{};
	if (sx == ex && sy == ey) return{};
	if (mCell[sx][sy] > 0 || mCell[ex][ey] > 0) return{};

	// 	13 ，35
	// 115，20
	if (mAstar->PathFinding(sx, sy, ex, ey))
	{
		return mAstar->GetMoveList();
	}
	else {
		return{};
	}

}

GameMap::~GameMap()
{
	if (mXyqMap != nullptr) {
		delete mXyqMap;
		mXyqMap = nullptr;
	}
}
void GameMap::Draw(SpriteRenderer* renderer,int offx,int offy)
{
	for (int i = 0; i<mRow; i++) {
		for (int j = 0; j<mCol; j++) {
			renderer->DrawSprite(mMapTiles[i*mCol + j],
				glm::vec2(j * 320 + offx,i * 240 + offy),
				glm::vec2(320, 240),
				0.0f,
				glm::vec3(1.0f, 1.0f, 1.0f)
			);
		}
	}
}


void GameMap::DrawCell(SpriteRenderer* renderer, int cur_x, int cur_y)
{
	for (int y = 0; y < mCellHeight; y++)
	{
		for (int x = 0; x < mCellWidth; x++)
		{
			if (mCell[x][y] >= 1)
			{
				// renderer->DrawSprite(mCellPic,
				// 	glm::vec2( x*20 + cur_x,
				// 		y*20 + cur_y),
				// 	glm::vec2(20, 20),
				// 	0.0f, glm::vec3(1.0f, 1.0f, 1.0f));
			}
		}
	}

}




void GameMap::DrawMask(SpriteRenderer* renderer, int offx, int offy)
{
	for (int m = 0; m < mXyqMap->m_MaskSize; m++)
	{
		NetEase::MaskInfo& info = mXyqMap->m_MaskInfos[m];
		renderer->DrawSprite(
			mMaskTiles[m],
			glm::vec2(info.StartX + offx,
				info.StartY  + offy),
			glm::vec2(info.Width, info.Height),
			0.0f, glm::vec3(1.0f, 1.0f, 1.0f));
	}
}


/*
int main()
{
GameMap gmap("1501.map");
gmap.Move(252, 60, 38, 16);
// Heap<Astar::Node*> heap;
// std::default_random_engine e;
// for(int i=0;i<100;i++)
// {
// 	int x = e()%317;
// 	Astar::Node* p = gmap.GetAstar()->NewNode(x,x);
// 	p->key = x;
// 	heap.add(p);
// }
// heap.print();
// heap.sort();
}
*/
