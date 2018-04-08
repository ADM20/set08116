#include <glm\glm.hpp>
#include <graphics_framework.h>

using namespace std;
using namespace graphics_framework;
using namespace glm;


//effects
effect eff;
effect sky_eff;
//cameras
target_camera cam;
free_camera fcam;
//lights
vector<point_light> points(4);
vector<spot_light> spots(5);
//cursor location
double cursor_x = 0.0;
double cursor_y = 0.0;
//choices
bool c1 = false;
bool c2 = true;
bool c3 = false;
bool c4 = false;
bool c5 = false;
bool c6 = false;
// mesh map
map<string, mesh> meshes;
//texture map
map<string, texture> tex;
//transform map
map<mesh*, mesh*>hierarchy; 
//skybox
cubemap sky_cube;
mesh skybox;

bool initialise() {
	//hide cursor
	glfwSetInputMode(renderer::get_window(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	// initial mouse position
	glfwGetCursorPos(renderer::get_window(), &cursor_x, &cursor_y);
	return true;
}
bool load_content()
{

	{
		//plane mesh
		meshes["plane"] = mesh(geometry_builder::create_plane());
		//skull mesh
		meshes["skull"] = mesh(geometry("models/skull3.obj"));
		//table mesh
		meshes["table"] = mesh(geometry("models/table.obj"));
	}

	{
		//my transforms
		meshes["plane"].get_transform().scale = (vec3(1, 1, 1));
		meshes["skull"].get_transform().scale = (vec3(0.5, 0.5, 0.5));
		meshes["skull"].get_transform().position = vec3(-25, 6.25, -25);
		meshes["table"].get_transform().scale = (vec3(0.1, 0.1, 0.1));
		meshes["table"].get_transform().position = vec3(-20, 1, -20);
	}

	{
		//Set materials
		material mat;
		//all emissive is black
		mat.set_emissive(vec4(0.0f, 0.0f, 0.0f, 1.0f));
		//all specular is white
		mat.set_specular(vec4(1.0f));
		//shininess is 20
		mat.set_shininess(2.0f);
	
		material mat2;
		//emissive
		mat2.set_emissive(vec4(0.0f, 0.0f, 0.0f, 1.0f));
		//specular
		mat2.set_specular(vec4(0.5f));
		//shininess
		mat2.set_shininess(0.5f);

		material mat3;
		//emissive
		mat2.set_emissive(vec4(0.0f, 0.0f, 0.0f, 1.0f));
		//specular
		mat2.set_specular(vec4(0.5f));
		//shininess
		mat2.set_shininess(1.0f);

		//table
		meshes["table"].set_material(mat);
		//skull
		meshes["skull"].set_material(mat2);
		meshes["plane"].set_material(mat3);
	}

	// Load texture
	{
		tex["floor"] = texture("textures/wood1.jpg", true, true);
		tex["skull"] = texture("textures/stone.jpg");
		tex["table1"] = texture("textures/wood1.jpg");
		tex["table2"] = texture("textures/wood2.jpg");

	}

	//Lights
	{
		// Set lighting values
		// Point 0, Position (-25, 5, -15)
		points[0].set_position(vec3(-25.0f, 10.0f, -10.0f));
		points[0].set_light_colour(vec4(1.0f, 1.0f, 1.0f, 1.0f));
		// Red, 20 range
		points[0].set_range(20.0f);
		// Point 1, Position (-25, 5, -35)
		points[1].set_position(vec3(-25.0f, 5.0f, -15.0f));
		points[1].set_light_colour(vec4(1.0f, 0.0f, 0.0f, 1.0f));
		// Red,20 range
		points[1].set_range(20.0f);
		// Point 2,Position (-10, 5, -15)
		points[2].set_position(vec3(-10.0f, 5.0f, -15.0f));
		points[2].set_light_colour(vec4(1.0f, 0.0f, 0.0f, 1.0f));
		// Red,20 range
		points[2].set_range(20.0f);
		// Point 3,Position (-10, 5, -35)
		points[3].set_position(vec3(-10.0f, 5.0f, -15.0f));
		points[3].set_light_colour(vec4(1.0f, 0.0f, 0.0f, 1.0f));
		// Red,20 range
		points[3].set_range(20.0f);
		// Spot 0, Position (-25, 10, -15)
		spots[0].set_position(vec3(-25.0f, 10.0f, -15.0f));
		spots[0].set_light_colour(vec4(0.0f, 1.0f, 0.0f, 1.0f));
		// Green, Direction (1, -1, -1) normalized
		spots[0].set_direction(normalize(vec3(1.0f, -1.0f, -1.0f)));
		// 20 range,0.5 power
		spots[0].set_range(20.0f);
		spots[0].set_power(0.5);
		// Spot 1,Position (-25, 10, -35)
		spots[1].set_position(vec3(-25.0f, 10.0f, -35.0f));
		spots[1].set_light_colour(vec4(0.0f, 1.0f, 0.0f, 1.0f));
		// Green,Direction (1, -1, 1) normalized
		spots[1].set_direction(normalize(vec3(1.0f, -1.0f, 1.0f)));
		// 20 range,0.5 power
		spots[1].set_range(20.0f);
		spots[1].set_power(0.5);
		// Spot 2,Position (-10, 10, -15)
		spots[2].set_position(vec3(-10.0f, 10.0f, -15.0f));
		spots[2].set_light_colour(vec4(0.0f, 1.0f, 0.0f, 1.0f));
		// Green,Direction (-1, -1, -1) normalized
		spots[2].set_direction(normalize(vec3(-1.0f, -1.0f, -1.0f)));
		// 20 range,0.5 power
		spots[2].set_range(20.0f);
		spots[2].set_power(0.5);
		// Spot 3,Position (-10, 10, -35)
		spots[3].set_position(vec3(-10.0f, 10.0f, -35.0f));
		spots[3].set_light_colour(vec4(0.0f, 1.0f, 0.0f, 1.0f));
		// Green,Direction (-1, -1, 1) normalized
		spots[3].set_direction(normalize(vec3(-1.0f, -1.0f, 1.0f)));
		// 20 range,0.5 power
		spots[3].set_range(20.0f);
		spots[3].set_power(0.5);
		// Spot 4,Position (-17.5, 15, -25)
		spots[4].set_position(vec3(-17.5f, 15.0f, -25.0f));
		spots[4].set_light_colour(vec4(0.0f, 1.0f, 0.0f, 1.0f));
		// Blue,Direction (0, -1, 0)
		spots[4].set_direction(normalize(vec3(0.0f, -1.0f, 0.0f)));
		// 30 range,1.0 power	
		spots[4].set_range(30.0f);
		spots[4].set_power(1);
	}
	
	{
		// Load in shaders
		eff.add_shader("AP_coursework/multi-light.vert", GL_VERTEX_SHADER);
		eff.add_shader("AP_coursework/multi-light.frag", GL_FRAGMENT_SHADER);

		//sky shaders
		sky_eff.add_shader("AP_coursework/skybox.vert", GL_VERTEX_SHADER);
		sky_eff.add_shader("AP_coursework/skybox.frag", GL_FRAGMENT_SHADER);


		// Build effect
		eff.build();
		sky_eff.build();
	}
	//skybox
	{
		skybox = mesh(geometry_builder::create_box());
		skybox.get_transform().scale = vec3(3000, 3000, 3000);
	}
	//skybox textures
	{
		array<string, 6> filenames = { "textures/brick.jpg","textures/brick.jpg","textures/brick.jpg","textures/brick.jpg","textures/brick.jpg","textures/brick.jpg" };
		sky_cube = cubemap(filenames);
	}
	{
		// Set target camera properties
		cam.set_position(vec3(50.0f, 10.0f, 50.0f));
		cam.set_target(vec3(-25.0f, 6.0f, -25.0f));
		cam.set_projection(quarter_pi<float>(), renderer::get_screen_aspect(), 0.1f, 1000.0f);
		
		// Set free camera properties
		fcam.set_position(vec3(50.0f, 10.0f, 50.0f));
		fcam.set_target(vec3(0.0f, 0.0f, 0.0f));
		fcam.set_projection(quarter_pi<float>(), renderer::get_screen_aspect(), 0.1f, 1000.0f);
		
	}
	return true;
}

bool update(float delta_time)

{
	//choose camera
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_1))
	{
		c1 = true;
		c2 = false;
		c3 = false;
		c4 = false;
		c5 = false;
		c6 = false;
		fcam.set_position(vec3(-20, 10, -10));

	}
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_2))
	{
		c1 = false;
		c2 = true;
		c3 = false;
		c4 = false;
		c5 = false;
		c6 = false;
	}
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_3))
	{
		c1 = false;
		c2 = false;
		c3 = true;
		c4 = false;
		c5 = false;
		c6 = false;
	}
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_4))
	{
		c1 = false;
		c2 = false;
		c3 = false;
		c4 = true;
		c5 = false;
		c6 = false;
	}
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_5))
	{
		c1 = false;
		c2 = false;
		c3 = false;
		c4 = false;
		c5 = true;
		c6 = false;
		fcam.set_position(vec3(-20, 10, 0));
	}
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_6))
	{
		c1 = false;
		c2 = false;
		c3 = false;
		c4 = false;
		c5 = false;
		c6 = true;
		fcam.set_position(vec3(-20, 10, 0));
	}
	
	
	//free cam
	if (c1)
	{
		
		// The ratio of pixels to rotation
		static double ratio_width = quarter_pi<float>() / static_cast<float>(renderer::get_screen_width());
		static double ratio_height =
			(quarter_pi<float>() *
			(static_cast<float>(renderer::get_screen_height()) / static_cast<float>(renderer::get_screen_width()))) /
			static_cast<float>(renderer::get_screen_height());

		double current_x;
		double current_y;

		// Get the current cursor position
		glfwGetCursorPos(renderer::get_window(), &current_x, &current_y);
		// Calculate delta of cursor positions from last frame
		double delta_x = current_x - cursor_x;
		double delta_y = current_y - cursor_y;
		// Multiply deltas by ratios - gets actual change in orientation
		delta_x *= ratio_width;
		delta_y *= ratio_height;
		// Rotate cameras by delta
		fcam.rotate(delta_x, -delta_y);
		// Use keyboard to move the camera - WSAD ctrl SPACE

		vec3 forward, back, left, right, up, down;
		forward = vec3(0, 0, 0);
		back = vec3(0, 0, 0);
		left = vec3(0, 0, 0);
		right = vec3(0, 0, 0);
		up = vec3(0, 0, 0);
		down = vec3(0);
		vec3 total = vec3(0);

		//take keyboard input
		if (glfwGetKey(renderer::get_window(), GLFW_KEY_W))
			forward = vec3(0.0f, 0.0f, 1.0f);
		if (glfwGetKey(renderer::get_window(), GLFW_KEY_S))
			back = vec3(0.0f, 0.0f, -1.0f);
		if (glfwGetKey(renderer::get_window(), GLFW_KEY_A))
			left = vec3(-1.0f, 0.0f, 0.0f);
		if (glfwGetKey(renderer::get_window(), GLFW_KEY_D))
			right = vec3(1.0f, 0.0f, 0.0f);
		if (glfwGetKey(renderer::get_window(), GLFW_KEY_SPACE)) {
			up = vec3(0, 1.0f, 0);
		}
		if (glfwGetKey(renderer::get_window(), GLFW_KEY_LEFT_CONTROL)) {
			left = vec3(0, -1, 0);
		}
		// Move camera
		total = forward + back + left + right + up + down;
		if (total != vec3(0))
			fcam.move(normalize(total)* delta_time * 10.0f);
		//update cam
		fcam.update(delta_time);
		// Update cursor pos
		glfwGetCursorPos(renderer::get_window(), &cursor_x, &cursor_y);

		skybox.get_transform().position = fcam.get_position();
	}
	
	//target cam pos 1
	if (c2)
	{
		cam.set_position(vec3(-50, 20, -10));
		cam.update(delta_time);
		skybox.get_transform().position = cam.get_position();
	}
	//pos 2
	if (c3)
	{
		cam.set_position(vec3(-50, 10, -50));
		cam.update(delta_time);
		skybox.get_transform().position = cam.get_position();
	}
	//pos 3
	if (c4)
	{
		cam.set_position(vec3(25, 10, -50));
		cam.update(delta_time);
		skybox.get_transform().position = cam.get_position();
	}


	//point light control
	if (c5)
	{
		if (glfwGetKey(renderer::get_window(), GLFW_KEY_W)) {
			points[0].move(vec3(0.0f, 0.0f, -1.0f));
		}
		if (glfwGetKey(renderer::get_window(), GLFW_KEY_S)) {
			points[0].move(vec3(0.0f, 0.0f, 1.0f));
		}
		if (glfwGetKey(renderer::get_window(), GLFW_KEY_A)) {
			points[0].move(vec3(-1.0f, 0.0f, 0.0f));
		}
		if (glfwGetKey(renderer::get_window(), GLFW_KEY_D)) {
			points[0].move(vec3(1.0f, 0.0f, 0.0f));
		}
		fcam.update(delta_time);
		skybox.get_transform().position = fcam.get_position();
	}
	//spot light control
	if (c6)
	{
		if (glfwGetKey(renderer::get_window(), GLFW_KEY_W)) {
			spots[0].move(vec3(0.0f, 0.0f, -1.0f));
		}
		if (glfwGetKey(renderer::get_window(), GLFW_KEY_S)) {
			spots[0].move(vec3(0.0f, 0.0f, 1.0f));
		}
		if (glfwGetKey(renderer::get_window(), GLFW_KEY_A)) {
			spots[0].move(vec3(-1.0f, 0.0f, 0.0f));
		}
		if (glfwGetKey(renderer::get_window(), GLFW_KEY_D)) {
			spots[0].move(vec3(1.0f, 0.0f, 0.0f));
		}
		fcam.update(delta_time);
		skybox.get_transform().position = fcam.get_position();
	}


	return true;
}

bool render() {

	// Create MVP matrix
	mat4 M;
	mat4 V;
	mat4 P;

	//check what camera is used
	if (c1);
	{
		V = fcam.get_view();
		P = cam.get_projection();
	}
	if (c2)
	{
		V = cam.get_view();
		P = cam.get_projection();
	}
	if (c3)
	{
		V = cam.get_view();
		P = cam.get_projection();
	}
	if (c4)
	{
		V = cam.get_view();
		P = cam.get_projection();
	}

	//render skybox
	{
		M = skybox.get_transform().get_transform_matrix();

		glDisable(GL_DEPTH_TEST);
		glDepthMask(GL_FALSE);
		glDisable(GL_CULL_FACE);

		renderer::bind(sky_eff);

		mat4 MVP = P * V * M;

		//cubemap uniform
		glUniformMatrix4fv(sky_eff.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MVP));
		renderer::bind(sky_cube, 0);
		glUniform1i(sky_eff.get_uniform_location("cubemap"), 0);
		renderer::render(skybox);
		cout << "skybox" << endl;

		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);
		glEnable(GL_CULL_FACE);

	}
	// Render meshes
	for (auto &e : meshes) {
		auto m = e.second;

		M = m.get_transform().get_transform_matrix();
		mat4 MVP = P * V * M;
		// Bind effect
		renderer::bind(eff);
		// Set MVP matrix uniform
		glUniformMatrix4fv(eff.get_uniform_location("MVP"),1,GL_FALSE,value_ptr(MVP));       
			// Set M matrix uniform
		glUniformMatrix4fv(eff.get_uniform_location("M"), 1, GL_FALSE, value_ptr(M));
		// Set N matrix uniform - remember - 3x3 matrix
		glUniformMatrix3fv(eff.get_uniform_location("N"), 1, GL_FALSE, value_ptr(m.get_transform().get_normal_matrix()));

		// Bind material
		renderer::bind(m.get_material(), "mat");
		renderer::bind(m.get_material(), "mat2");
		// Bind point lights
		renderer::bind(points, "points");
		// Bind spot lights
		renderer::bind(spots, "spots");

		// Bind texture
		if (e.first == "skull")
		{
			renderer::bind(tex["skull"], 0);
			glUniform1i(eff.get_uniform_location("tex"), 0);
			cout << e.first << endl;
		}
		if (e.first == "table")
		{
			renderer::bind(tex["table1"], 2);
			renderer::bind(tex["table2"], 3);
			glUniform1i(eff.get_uniform_location("tex"), 1);
			cout << e.first << endl;
		}

		if (e.first == "plane")
		{
			//bind actual texture
			renderer::bind(tex["floor"], 0);
			//set tex uniform
			glUniform1i(eff.get_uniform_location("tex"), 0);
			if (c1)
				glUniform3fv(eff.get_uniform_location("eye_pos"), 1, value_ptr(fcam.get_position()));
			else 
				glUniform3fv(eff.get_uniform_location("eye_pos"), 1, value_ptr(cam.get_position()));
		
			cout << e.first << endl;
		}

		

		// Set tex uniform
		glUniform1i(eff.get_uniform_location("tex"), 0);
		// Set eye position- Get this from active camera
		if (c1)
			glUniform3fv(eff.get_uniform_location("eye_pos"), 1, value_ptr(fcam.get_position()));
		else
			glUniform3fv(eff.get_uniform_location("eye_pos"), 1, value_ptr(cam.get_position()));
		
		// Render mesh
		renderer::render(m);
	}

	return true;
}

void main() {
	// Create application
	app application("AP_coursework");
	// Set load content, update and render methods
	application.set_initialise(initialise);
	application.set_load_content(load_content);
	application.set_update(update);
	application.set_render(render);
	// Run application
	application.run();
}