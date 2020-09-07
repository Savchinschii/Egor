class CRect {
  public :
    CRect()
        { x1 = y1 = x2 = y2 = 0; }
    CRect( int nx1, int ny1, int nx2, int ny2 )
        { x1 = nx1; y1 = ny1; x2 = nx2; y2 = ny2; }
     
     void SetRect( int left, int top, int right, int bottom )
         { x1 = left; y1 = top; x2 = right; y2 = bottom; }
     
     void OffsetRect( int dx, int dy )
         { x1 += dx; y1 += dy; x2 += dx; y2 += dy; }
     
     void CenterPoint( int* x, int* y )
         { *x = (x1 + x2)/2; *y = (y1 + y2)/2; }
     
     bool IsRectEmpty()
         { return ( x1 == x2 && y1 == y2 ); }
     
     int Width()      { return x2 - x1 + 1; }
     
     int Height()     { return y2 - y1 + 1; }
     
     CRect IntersectRect( CRect& another );
     
     void NormalizeRect();
     
     int x1, y1, x2, y2;
};
class CWall {
  public:
    CWall( int left, int top, int right, int bottom,
           double cf, CWall* pNextWall );
       
       void Draw();
      
       void HitBy( CBall* pBall );
       CRect GetRegion()        { return region; }
       CWall* GetLink()         { return pLink; }
     private:
       CWall* pLink;
       CRect region;            
       double convertFactor;    
};
CWall::CWall( int left, int top, int right, int bottom,
              double cf, CWall* pNextWall )
{
  convertFactor = cf;
  region.SetRect( left, top, right, bottom );
  pLink = pNextWall;
}
void CWall::Draw()
{
  glColor3ub( 0, 255, 0 );
  glRecti( region.x1, region.y1, region.x2, region.y2 );
}
void CWall::HitBy( CBall* pBall )
{
  pBall->SetDirection( convertFactor - pBall->GetDirection() );
}
class CHole {
  public:
    CHole( int x, int y, CHole* pNextHole );
       // Рисование лузы
       void Draw();
       // Извещение лузы о том, что в нее попал шар
       void HitBy( CBall* pBall );
       CRect GetRegion() { return region; }
       CHole* GetLink() { return pLink; }
     private:
       CHole* pLink;    // Указатель на следующую лузу для образования
                        // связного списка
       CRect region;    // Экранные координаты области лузы
};
CHole::CHole( int x, int y, CHole* pNextHole )
{
    region.SetRect( x - 5, y - 5, x + 5, y + 5 );
    pLink = pNextHole;
}

void CHole::Draw()
{
    // Луза рисуется в виде желтого круга, вписанного в область region
    glColor3ub( 255, 255, 0 );
    glPointSize( (float)region.Width() );
    glEnable( GL_POINT_SMOOTH );
    glBegin( GL_POINTS );
      int cx, cy;
      region.CenterPoint( &cx, &cy );
      glVertex2i( cx, cy );
    glEnd();
    glDisable( GL_POINT_SMOOTH );
}
void CHole::HitBy( CBall* pBall )
{
    
    pBall->SetEnergy( 0.0 );
       if ( pBall->IsCue() )
         pBall->SetCenter( 50, 108 );
       else
         {



         pBall->SetCenter( 10 + saveRack*15, 250 );
         saveRack++;     // Увеличение глобального счетчика забитых шаров
         }
}
class CBall {
  public:
    CBall( int x, int y, bool fc, CBall* pNextBall );
       
       void Draw();
       
       void Update();
       // Извещение шара о том, что в него попал другой шар
       void HitBy( CBall* pBall );
       // Расчет угла между осью OX и направлением от центра шара до точки,
       // смещенной от центра шара на (dx, dy)
       double HitAngle( double dx, double dy );
       
       void   SetEnergy( double v )            {   energy = v; }
       void   SetCenter( int newx, int newy );
       void   SetDirection( double newDir )    {   direction = newDir; };
       CRect GetRegion()                       {   return region; }
       CBall* GetLink()                        {   return pLink; }
       double GetEnergy()                      {   return energy; }
       void   GetCenter( int* x, int* y );
       double GetDirection()                   {   return direction; }
       bool   IsCue()                          {   return fCue; }
     private:
       CBall* pLink;     
       CRect region;     
       double direction; 
                         
       double energy;    
       bool   fCue;      
};
CBall::CBall( int x, int y, bool fc, CBall* pNextBall )
{
  SetCenter( x, y );
  SetDirection( 0 );
  SetEnergy( 0.0 );
  pLink = pNextBall;
  fCue = fc;
}
void CBall::Draw()
{
  if ( IsCue() )
    glColor3ub( 255, 255, 255 );        
  else
    glColor3ub( 0, 0, 255 );            
    glPointSize( (float)region.Width() );
    glEnable( GL_POINT_SMOOTH );
    glBegin( GL_POINTS );
      int cx, cy;
      region.CenterPoint( &cx, &cy );
      glVertex2i( cx, cy );
    glEnd();
    glDisable( GL_POINT_SMOOTH );
}
void CBall::Update()
{
  // Для движения у шара должна быть некоторая кинетическая энергия
  if ( energy <= 0.5 )
    return;
    fBallMoved = true;
    
    energy -= 0.05;
    
    int dx = (int)( 2.0*sqrt(energy)*cos(direction) );
    int dy = (int)( 2.0*sqrt(energy)*sin(direction) );
    region.OffsetRect( dx, dy );
    // Проверка на попадание в лузу
    CHole* hptr = listOfHoles;
    while ( hptr )
      {
      CRect is = region.IntersectRect( hptr->GetRegion() );
      if ( !is.IsRectEmpty() )
        {
        hptr->HitBy( this );
        hptr = NULL;
        }
      else

                                       


          hptr = hptr->GetLink();
      }
    // Проверка на попадание в стенку
    CWall* wptr = listOfWalls;
    while ( wptr )
      {
      CRect is = region.IntersectRect( wptr->GetRegion() );
      if ( !is.IsRectEmpty() )
        {
        wptr->HitBy( this );
        wptr = NULL;
        }
      else
        wptr = wptr->GetLink();
      }
    // Проверка на попадание в другой шар
    CBall* bptr = listOfBalls;
    while ( bptr )
      {
      if ( bptr != this )
        {
        CRect is = region.IntersectRect( bptr->GetRegion() );
        if ( !is.IsRectEmpty() )
          {
          bptr->HitBy( this);
          break;
          }
        }
      bptr = bptr->GetLink();
      }
}
void CBall::HitBy( CBall* pBall )
{
  pBall->SetEnergy( pBall->GetEnergy()/2.0 );
  
  energy += pBall->GetEnergy();
    
    int cx1, cy1, cx2, cy2;
    GetCenter( &cx1, &cy1 );
    pBall->GetCenter( &cx2, &cy2 );
    SetDirection( HitAngle( cx1 - cx2, cy1 - cy2 ) );
    
    double da = pBall->GetDirection() - GetDirection();
    pBall->SetDirection( pBall->GetDirection() + da );
}

// Расчет угла между осью OX и вектором (dx, dy). Функция возвращает
// значение угла радианах в диапазоне (0,PI) или (-PI,0)
double CBall::HitAngle( double dx, double dy )

                                    


{
    double na;
    if ( fabs(dx) < 0.05 )
      na = PI/2;
    else
      na = atan( fabs(dy/dx) );
    if (   dx < 0 )
      na   = PI - na;
    if (   dy < 0 )
      na   = -na;
    return na;
}
void CALLBACK MouseButtonDown( AUX_EVENTREC* event )
{
  
  int mouse_x = event->data[0];
  int mouse_y = WINDOW_HEIGHT - event->data[1];
    
    pCueBall->SetEnergy( 20.0 );
    
    int cx, cy;
    pCueBall->GetCenter( &cx, &cy );
    pCueBall->SetDirection( pCueBall->HitAngle( mouse_x-cx, mouse_y-cy ) );
    
    fBallMoved = true;
}
// Фоновая функция
void CALLBACK Idle()
{
  
  if ( !fBallMoved )

                                   


      return;
   
    fBallMoved = false;
    CBall* bptr = listOfBalls;
    while ( bptr )
      {
      bptr->Update();
      bptr = bptr->GetLink();
      }
    Display();    
    Sleep( 25 );   // Задержка на 25 мс
    void CALLBACK Display()
{
  glClear( GL_COLOR_BUFFER_BIT );
    
    CWall* pWall = listOfWalls;
    while ( pWall )
      {
      pWall->Draw();
      pWall = pWall->GetLink();
      }
    
    CHole* pHole = listOfHoles;
    while ( pHole )
      {
      pHole->Draw();
      pHole = pHole->GetLink();
      }
    
    CBall* pBall = listOfBalls;
    while ( pBall )
      {
      pBall->Draw();
      pBall = pBall->GetLink();
      }
    auxSwapBuffers();
}
