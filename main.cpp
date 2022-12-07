#include <bits/stdc++.h>
#include <GL/glut.h>

using namespace std;

#define unused(x) (void)x

static double width = 900, height = 600, level_width = 45, level_height = 30;
static double cell_width, cell_height;
const static size_t snake_st_len = 6;
static float time_ms;
static size_t frame_skip = 10, frame_skip_count = 0, score;

static pair<int,int> snake_direction, food_pos;
static vector<pair<int,int>> snake, food_candidate;

static const float back_col[4] = { 0.824, 0.412, 0.706, 1.0 },
			 snak_col[4] = { 0.51, 0.255, 0.125, 1.0 },
			 food_col[4] = { 0.537, 0.769, 0.886, 1.0 },
			 text_col[4] = { 1.000, 1.000, 0.000, 1.0 };

int state_game_play = 0, state_game_over = 1;
int state_current;

static void draw_current_game_state ( void )
{
	// draw background
	glColor4fv ( back_col );
	glBegin ( GL_QUADS );
		glVertex2d ( 0, 0 );
		glVertex2d ( width, 0 );
		glVertex2d ( width, height );
		glVertex2d ( 0, height );
	glEnd ();

	// draw food
	glColor4fv ( food_col );
	glBegin ( GL_QUADS );
		glVertex2d ( cell_width * food_pos.first, cell_height * food_pos.second );
		glVertex2d ( cell_width * food_pos.first + cell_width, cell_height * food_pos.second );
		glVertex2d ( cell_width * food_pos.first + cell_width, cell_height * food_pos.second + cell_height );
		glVertex2d ( cell_width * food_pos.first, cell_height * food_pos.second + cell_height );
	glEnd ();

	// drawing snake
	glColor4fv ( snak_col );
	for ( size_t i = 0 ; i < snake.size() ; ++i ) {
		pair<int,int>& scell = snake[i];
		glBegin ( GL_QUADS );
			glVertex2d ( cell_width * scell.first, cell_height * scell.second );
			glVertex2d ( cell_width * scell.first, cell_height * scell.second + cell_height );
			glVertex2d ( cell_width * scell.first + cell_width, cell_height * scell.second + cell_height );
			glVertex2d ( cell_width * scell.first + cell_width, cell_height * scell.second );
		glEnd ();
	}
}

static void game_play_continue ( void )
{
	// moving snake
	if ( frame_skip_count++ >= frame_skip ) {
		for ( size_t i = 0 ; i < snake.size() - 1 ; ++i ) snake[i] = snake[i + 1];
		snake.back().first += snake_direction.first;
		snake.back().second += snake_direction.second;
		if ( snake.back().first >= level_width ) snake.back().first = 0;
		if ( snake.back().first < 0 ) snake.back().first = level_width - 1;
		if ( snake.back().second >= level_height ) snake.back().second = 0;
		if ( snake.back().second < 0 ) snake.back().second = level_height - 1;
		frame_skip_count -= frame_skip;
	}

	// collision with food
	if ( snake.back() == food_pos ) {
		food_pos = food_candidate.back();
		food_candidate.pop_back();
		snake.insert ( snake.begin(), snake.end()[-2] );
		score += rand() % 3;
	}
	// game over if no food left
	if ( food_candidate.empty() ) state_current = state_game_over;

	// collision with snake
	for ( size_t i = 0 ; i < snake.size() - 1 ; ++i )
		if ( snake[i] == snake.back() )
			state_current = state_game_over;
	draw_current_game_state ();
}

static void game_over_for_you ( void )
{
	draw_current_game_state ();
	glColor4f ( text_col[0], text_col[1], text_col[2], 1.0 );
	string str_score = "Your total socre is : " + to_string ( score );
	glRasterPos2d ( width / 2 - 9 * str_score.size()/2, height / 2 );
	glBegin ( GL_POINTS );
		glVertex2d ( 0, 0 );
	glEnd ();
	for ( auto c : str_score )
		glutBitmapCharacter ( GLUT_BITMAP_HELVETICA_18, c );
	string next_gm = "Press Any arrow key to continue";
	glRasterPos2d ( width / 2 - 9 * next_gm.size()/2, height / 2 + 18 );
	for ( auto c : next_gm )
		glutBitmapCharacter ( GLUT_BITMAP_HELVETICA_18, c );
}

static void display_func ( void )
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glLoadIdentity();
	height = glutGet ( GLUT_WINDOW_HEIGHT ); width = glutGet ( GLUT_WINDOW_WIDTH );
	gluOrtho2D ( 0.0, width, height, 0.0 );
	time_ms = ((float)glutGet ( GLUT_ELAPSED_TIME ) / 1000.0f );
	cell_width = width / level_width; cell_height = height / level_height;

	if ( state_current == state_game_play ) game_play_continue ();
	else if ( state_current == state_game_over ) game_over_for_you ();

	glutSwapBuffers();
}

static void idle ( void ) { glutPostRedisplay (); }

void initialize_all ( void )
{
	score = 0;
	state_current = state_game_play;
	snake_direction = make_pair<int,int> ( 1, 0 );
	snake.clear();
	for ( size_t i = 0 ; i < snake_st_len ; ++i )
		snake.push_back ( make_pair<int,int>(
					level_width/2 + i - snake_st_len/2,
					level_height/2 )
				);
	for ( size_t i = 0 ; i < level_width ; ++i ) {
		for ( size_t j = 0 ; j < level_height ; ++j ) {
			if ( count (
						snake.begin(), snake.end(),
						make_pair<int,int>( i, j ) )
					> 0 )
				continue;
			food_candidate.push_back ( make_pair<int,int>( i, j ) );
		}
	}
	random_shuffle ( food_candidate.begin(), food_candidate.end() );
	food_pos = food_candidate.back();
	food_candidate.pop_back();
}

void cleanup_all ( void )
{
}

void arrow_keys ( int key, int x, int y )
{
	unused ( x ); unused ( y );
	switch ( key ) {
		case GLUT_KEY_UP:
			if ( snake_direction.second != 1 ) {
				snake_direction.second = -1;
				snake_direction.first = 0;
			}
			break;
		case GLUT_KEY_DOWN:
			if ( snake_direction.second != -1 ) {
				snake_direction.second = 1;
				snake_direction.first = 0;
			}
			break;
		case GLUT_KEY_LEFT:
			if ( snake_direction.first != 1 ) {
				snake_direction.first = -1;
				snake_direction.second = 0;
			}
			break;
		case GLUT_KEY_RIGHT:
			if ( snake_direction.first != -1 ) {
				snake_direction.first = 1;
				snake_direction.second = 0;
			}
			break;
	}
	switch ( key ) {
		case GLUT_KEY_UP:
		case GLUT_KEY_DOWN:
		case GLUT_KEY_LEFT:
		case GLUT_KEY_RIGHT:
			if ( state_current == state_game_over ) initialize_all ();
			break;
	}
}

int main ( int argc, char* argv[] )
{
	glutInit ( &argc, argv );
	glutInitWindowSize ( width, height );
	glutInitWindowPosition ( 400, 25 );
	glutInitDisplayMode ( GLUT_RGBA | GLUT_ALPHA | GLUT_DOUBLE);

	glutCreateWindow ( "Simple Snake" );

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable( GL_BLEND );
	glClearColor(0.0,0.0,0.0,0.0);

	glutDisplayFunc ( display_func );
	glutIdleFunc ( idle );
	glutSpecialFunc ( arrow_keys );

	srand ( time(0) );
	initialize_all ();
	atexit ( cleanup_all );
	glutMainLoop();

	return 0;
}
