#include "stdafx.h"
#include "CCharacterRaceWindowHandler.h"


CCharacterRaceWindowHandler::CCharacterRaceWindowHandler()
{
	pWindow = std::make_shared<UI::Window>( Rectangle2D( (RESOLUTION_WIDTH >> 1) - (588 >> 1), (RESOLUTION_HEIGHT >> 2), 588, 370 ) );
}


CCharacterRaceWindowHandler::~CCharacterRaceWindowHandler()
{
}

void CCharacterRaceWindowHandler::Init()
{
	UI::Window_ptr pWindowTempskron = std::make_shared<UI::Window>( Rectangle2D( 0, 0, 128, 164 ) );
	pWindowTempskron->SetID( WINDOW_Tempskron );
	pWindow->AddElement( pWindowTempskron );
	
	//UI::Button_ptr pButtonImageTempskron = std::make_shared<UI::Button>( Rectangle2D( 0, 0, 128, 128 ) );
	//pButtonImageTempskron->SetImage( UI::ImageLoader::LoadImage( GetImagePath( "mark_T.png" ).c_str() ) );
	//pButtonImageTempskron->SetAlphaHover( 180 );
	//pWindowTempskron->AddElement( pButtonImageTempskron );

	UI::Button_ptr pButtonImageTitleTempskron = std::make_shared<UI::Button>( Rectangle2D( 0, 0, 128, 164 ) );
	pButtonImageTitleTempskron->SetImage( UI::ImageLoader::LoadImage( GetImagePath( "t-tempskron_icon.png" ).c_str() ) );
	pButtonImageTitleTempskron->SetAlphaHover( 180 );
	pWindowTempskron->AddElement( pButtonImageTitleTempskron );

	UI::ImageBox_ptr pImageTempskronBox = std::make_shared<UI::ImageBox>( Rectangle2D( 0, 166, 290, 230 ) );
	pImageTempskronBox->SetID( IMAGE_Tempskronbox );
	pImageTempskronBox->SetImage( UI::ImageLoader::LoadImage( GetImagePath( "text_tempskron_box.png" ).c_str() ) );
	pImageTempskronBox->Hide();
	pWindow->AddElement( pImageTempskronBox );

	UI::Button_ptr pButtonTempskron = std::make_shared<UI::Button>( Rectangle2D( 0, 0, 128, 164 ) );
	pButtonTempskron->SetID( BUTTON_Tempskron );
	pButtonTempskron->SetOnHoverInEvent( UI::Event::Build( std::bind( &CCharacterRaceWindowHandler::OnHoverTempskronButton, this, true, std::placeholders::_1 ) ) );
	pButtonTempskron->SetOnHoverOutEvent( UI::Event::Build( std::bind( &CCharacterRaceWindowHandler::OnHoverTempskronButton, this, false, std::placeholders::_1 ) ) );
	pButtonTempskron->SetEvent( UI::Event::Build( std::bind( &CCharacterRaceWindowHandler::OnTempskronButtonClick, this, std::placeholders::_1 ) ) );
	pWindowTempskron->AddElement( pButtonTempskron );
	
	UI::Window_ptr pWindowMorion = std::make_shared<UI::Window>( Rectangle2D( 162, 0, 128, 164 ) );
	pWindowMorion->SetID( WINDOW_Morion );
	pWindow->AddElement( pWindowMorion );
	
	//UI::Button_ptr pButtonImageMorion = std::make_shared<UI::Button>( Rectangle2D( 0, 0, 128, 128 ) );
	//pButtonImageMorion->SetImage( UI::ImageLoader::LoadImage( GetImagePath( "mark_M.png" ).c_str() ) );
	//pButtonImageMorion->SetAlphaHover( 180 );
	//pWindowMorion->AddElement( pButtonImageMorion );
	
	UI::Button_ptr pButtonImageTitleMorion = std::make_shared<UI::Button>( Rectangle2D( 0, 0, 128, 164 ) );
	pButtonImageTitleMorion->SetImage( UI::ImageLoader::LoadImage( GetImagePath( "t-morion_icon.png" ).c_str() ) );
	pButtonImageTitleMorion->SetAlphaHover( 180 );
	pWindowMorion->AddElement( pButtonImageTitleMorion );

	UI::ImageBox_ptr pImageMorionBox = std::make_shared<UI::ImageBox>( Rectangle2D( pWindow->GetWidth() - 290, 166, 290, 230));
	pImageMorionBox->SetID( IMAGE_Morionbox );
	pImageMorionBox->SetImage( UI::ImageLoader::LoadImage( GetImagePath( "text_morion_box.png" ).c_str() ) );
	pImageMorionBox->Hide();
	pWindow->AddElement( pImageMorionBox );

	UI::Button_ptr pButtonMorion = std::make_shared<UI::Button>( Rectangle2D( 0, 0, 128, 164 ) );
	pButtonMorion->SetID( BUTTON_Morion );
	pButtonMorion->SetOnHoverInEvent( UI::Event::Build( std::bind( &CCharacterRaceWindowHandler::OnHoverMorionButton, this, true, std::placeholders::_1 ) ) );
	pButtonMorion->SetOnHoverOutEvent( UI::Event::Build( std::bind( &CCharacterRaceWindowHandler::OnHoverMorionButton, this, false, std::placeholders::_1 ) ) );
	pButtonMorion->SetEvent( UI::Event::Build( std::bind( &CCharacterRaceWindowHandler::OnMorionButtonClick, this, std::placeholders::_1 ) ) );
	pWindowMorion->AddElement( pButtonMorion );

	OnResolutionChanged();

	pWindow->Hide();
}

void CCharacterRaceWindowHandler::Render()
{
	pWindow->Render( 0, 0, RESOLUTION_WIDTH, RESOLUTION_HEIGHT, 0, 0 );
}

BOOL CCharacterRaceWindowHandler::OnMouseClick( CMouse * pcMouse )
{
	if ( pWindow->IsOpen() )
		return pWindow->OnMouseClick( pcMouse, 0, 0, RESOLUTION_WIDTH, RESOLUTION_HEIGHT, 0, 0 );

	return FALSE;
}

void CCharacterRaceWindowHandler::OnMouseMove( CMouse * pcMouse )
{
	pWindow->OnMouseMove( pcMouse, 0, 0, RESOLUTION_WIDTH, RESOLUTION_HEIGHT, 0, 0 );
}

void CCharacterRaceWindowHandler::OnResolutionChanged()
{
	pWindow->SetPosition( (RESOLUTION_WIDTH >> 1) - (pWindow->GetWidth() >> 1), (RESOLUTION_HEIGHT >> 2));

	UpdateWindowPosition();
}

void CCharacterRaceWindowHandler::Open()
{
	pWindow->Show();
}

void CCharacterRaceWindowHandler::Close()
{
	pWindow->Hide();

	OnHoverTempskronButton( false, UIEventArgs{} );
	OnHoverMorionButton( false, UIEventArgs{} );
}

void CCharacterRaceWindowHandler::OnTempskronButtonClick( UIEventArgs eArgs )
{
	GetCharacterScreen()->SetCharacterScreenStep( CHARACTERSCREENSTEP_NewCharacterTempskronClass );
}

void CCharacterRaceWindowHandler::OnMorionButtonClick( UIEventArgs eArgs )
{
	GetCharacterScreen()->SetCharacterScreenStep( CHARACTERSCREENSTEP_NewCharacterMorionClass );
}

std::string CCharacterRaceWindowHandler::GetImagePath( const std::string & strName )
{
	return ((CCharacterScreen *)GAMESCREEN->GetActiveScreen())->GetWindow()->GetImagePath( strName );
}

void CCharacterRaceWindowHandler::OnHoverTempskronButton( bool bHover, UIEventArgs eArgs )
{
	auto pTempskronBox = pWindow->GetElement<UI::ImageBox>( IMAGE_Tempskronbox );

	if ( bHover )
		pTempskronBox->Show();
	else
		pTempskronBox->Hide();
}

void CCharacterRaceWindowHandler::OnHoverMorionButton( bool bHover, UIEventArgs eArgs )
{
	auto pMorionBox = pWindow->GetElement<UI::ImageBox>( IMAGE_Morionbox );

	if ( bHover )
		pMorionBox->Show();
	else
		pMorionBox->Hide();
}

void CCharacterRaceWindowHandler::UpdateWindowPosition()
{
	auto pTempskronWindow = pWindow->GetElement<UI::Window>( WINDOW_Tempskron );
	auto pMorionWindow = pWindow->GetElement<UI::Window>( WINDOW_Morion );

	pTempskronWindow->SetPosition( (pWindow->GetWidth() >> 1) - pTempskronWindow->GetWidth() - 10, 0);
	pMorionWindow->SetPosition( (pWindow->GetWidth() >> 1) + 10, 0);
}

CCharacterScreen * CCharacterRaceWindowHandler::GetCharacterScreen()
{
	return ((CCharacterScreen *)GAMESCREEN->GetActiveScreen());
}
