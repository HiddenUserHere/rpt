#include "stdafx.h"
#include "UIImageBox.h"

namespace UI
{
ImageBox::ImageBox(Rectangle2D rRectangle) : Element(rRectangle)
{
}

ImageBox::~ImageBox()
{
}

void ImageBox::Clear()
{
	CLEARPTR( pImage );
	UI::ImageLoader::DeleteImage( pImage );

	if ( pImageActive1 != pImage )
	{
		CLEARPTR( pImageActive1 );
		UI::ImageLoader::DeleteImage( pImageActive1 );
	}

	if ( pImageActive2 != pImage )
	{
		CLEARPTR( pImageActive2 );
		UI::ImageLoader::DeleteImage( pImageActive2 );
	}
}

void ImageBox::SetImage( class UI::Image * pImage )
{
	this->pImage = pImage;
}

void ImageBox::SetActiveImage( std::pair<class UI::Image *, class UI::Image *> pPair )
{
	pImageActive1 = pPair.first;
	pImageActive2 = pPair.second;

	this->pImage = pImageActive1;
}

void ImageBox::SetGroup( std::shared_ptr<UI::Group> pGroupMain )
{
	pGroup = pGroupMain;
}

void ImageBox::Swap()
{
	/*if ( pImage )
	{
		pImage = UI::ImageLoader::Swap( pImage->GetPointer() );
	}
	if ( pImageActive1 )
	{
		pImageActive1 = UI::ImageLoader::Swap( pImageActive1->GetPointer() );
	}
	if ( pImageActive2 )
	{
		pImageActive2 = UI::ImageLoader::Swap( pImageActive2->GetPointer() );
	}*/

	if( pImage == pImageActive1 )
		pImage = pImageActive2;
	else
		pImage = pImageActive1;
}

void ImageBox::Swap( BOOL bActiveImage )
{
	pImage = bActiveImage ? pImageActive2 : pImageActive1;
}

void ImageBox::Render( int iX, int iY, int iWidth, int iHeight, int iSourceX, int iSourceY )
{
	if ( IsOpen() )
	{
		int iRenderX = GetX() + iX;
		int iRenderY = GetY() + iY;
		int iRenderWidth = low( GetWidth(), iWidth - GetX() );
		int iRenderHeight = low( GetHeight(), iHeight - GetY() );
		
		if ( psTexture )
		{
			UI::ImageRender::Render( psTexture, iRenderX, iRenderY, rRect.iWidth, rRect.iHeight, iSourceX, iSourceY, dwColor, Element::GetScale() );
		}
		else
		{
			if ( eRepeatImageMode == Horizontal )
			{
				UINT iRepeatCount	= rRect.iWidth / pImage->GetWidth();
				UINT iWidthLast		= rRect.iWidth % pImage->GetWidth();

				for ( UINT i = 0; i < iRepeatCount; i++ )
					UI::ImageRender::Render( pImage, iRenderX + (i * pImage->GetWidth()), iRenderY, pImage->GetWidth(), rRect.iHeight, iSourceX, iSourceY, dwColor, Element::GetScale() );

				if ( iWidthLast > 0 )
				{
					UI::ImageRender::Render( pImage, iRenderX + (iRepeatCount * pImage->GetWidth()), iRenderY, iWidthLast, rRect.iHeight, iSourceX, iSourceY, dwColor, Element::GetScale() );
				}
			}
			else if ( eRepeatImageMode == Vertical )
			{
				UINT iRepeatCount	= rRect.iHeight / pImage->GetHeight();
				UINT iHeightLast	= rRect.iHeight % pImage->GetHeight();

				for ( UINT i = 0; i < iRepeatCount; i++ )
					UI::ImageRender::Render( pImage, iRenderX, iRenderY + (i * pImage->GetHeight()), rRect.iWidth, pImage->GetHeight(), iSourceX, iSourceY, dwColor, Element::GetScale());

				if ( iHeightLast > 0 )
				{
					UI::ImageRender::Render( pImage, iRenderX, iRenderY + (iRepeatCount * pImage->GetHeight()), rRect.iWidth, iHeightLast, iSourceX, iSourceY, dwColor, Element::GetScale() );
				}
			}
			else
			{
				UI::ImageRender::Render( pImage, iRenderX, iRenderY, rRect.iWidth, rRect.iHeight, iSourceX, iSourceY, dwColor, Element::GetScale() );
			}
		}

		Element::Render( iRenderX, iRenderY, iRenderWidth, iRenderHeight, iSourceX, iSourceY );
	}
}
}