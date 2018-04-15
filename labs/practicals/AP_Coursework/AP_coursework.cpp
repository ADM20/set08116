#include <glm\glm.hpp>
#include <graphics_framework.h>

using namespace std;
using namespace std::chrono;
using namespace graphics_framework;
using namespace glm;

//smoke///////////
const unsigned int MAX_PARTICLES = 4096;
vec4 positions[MAX_PARTICLES];
vec4 velocitys[MAX_PARTICLES];
GLuint G_Position_buffer, G_Velocity_buffer;
effect smoke_eff;
effect compute_eff;
GLuint vao;
texture smoke_tex;
bool first_frame = true;
///////////////

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
	//smoke
	////////////////////////////////////////////////////////////////////////

	default_random_engine rand(duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count());
	uniform_real_distribution<float> dist;
	
	smoke_tex = texture("textures/smoke.png");

	//particles
	for (unsigned int i = 0; i < MAX_PARTICLES; ++i) 
	{
		positions[i] = vec4(((2.0f * dist(rand)) - 1.0f) / 10.0f, 5.0 * dist(rand), 0.0f, 0.0f);
		velocitys[i] = vec4(0.0f, 0.1f + dist(rand), 0.0f, 0.0f);
	}
	//load smoke shaders
	smoke_eff.add_shader("AP_coursework/smoke.vert", GL_VERTEX_SHADER);
	smoke_eff.add_shader("AP_coursework/smoke.frag", GL_FRAGMENT_SHADER);
	smoke_eff.add_shader("AP_coursework/smoke.geom", GL_GEOMETRY_SHADER);

	smoke_eff.build();

	compute_eff.add_shader("AP_coursework/particle.comp", GL_COMPUTE_SHADER);
	compute_eff.build();

	// a useless vao, but we need it bound or we get errors.
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	// *********************************
	
	//Generate Position Data buffer
	glGenBuffers(1, &G_Position_buffer);
	// Bind as GL_SHADER_STORAGE_BUFFER
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, G_Position_buffer);
// Send Data to GPU, use GL_DYNAMIC_DRAW
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(positions[0]) * MAX_PARTICLES, positions, GL_DYNAMIC_DRAW);
	// Generate Velocity Data buffer
	glGenBuffers(1, &G_Velocity_buffer);
	// Bind as GL_SHADER_STORAGE_BUFFER
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, G_Velocity_buffer);
	//draw
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(velocitys[0]) * MAX_PARTICLES, velocitys, GL_DYNAMIC_DRAW);
	
	// *********************************
	//Unbind
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	/////////////////////////////////////////////////////////////////////////////////
	{
		//plane mesh
		meshes["plane"] = mesh(geometry_builder::create_plane());
		//skull mesh
		meshes["skull"] = mesh(geometry("models/skull.obj"));
		//table mesh
		meshes["table"] = mesh(geometry("models/table.obj"));
		//bottle mesh
		meshes["bottle1"] = mesh(geometry("models/glass_bottle.obj"));
		//box mesh
		meshes["box"] = mesh(geometry("models/box1.obj"));
		//chair mesh
		meshes["chair"] = mesh(geometry("models/chair.obj"));
		

	}

	{
		//my transforms
		meshes["plane"].get_transform().scale = (vec3(1, 1, 1));
		meshes["skull"].get_transform().scale = (vec3(0.5, 0.5, 0.5));
		meshes["skull"].get_transform().position = vec3(-18, 6, -18);
		meshes["table"].get_transform().scale = (vec3(3, 3, 3));
		meshes["table"].get_transform().position = vec3(-20, 0, -20);
		meshes["bottle1"].get_transform().scale = (vec3(1, 1, 1));
		meshes["bottle1"].get_transform().position = vec3(-18, 6, -18);
		meshes["box"].get_transform().scale = (vec3(0.5, 0.5, 0.5));
		meshes["box"].get_transform().position = vec3(-25, 7, -18);
		meshes["chair"].get_transform().scale = (vec3(0.85, 0.85, 0.85));
		meshes["chair"].get_transform().position = vec3(-30, -1.5, -22);
	
	}

	{
		//Set materials
		material mat;
		//emissive 
		mat.set_emissive(vec4(0.0f, 0.0f, 0.0f, 1.0f));
		// specular 
		mat.set_specular(vec4(1.0f));
		//shininess 
		mat.set_shininess(20.0f);
	
		material mat2;
		//emissive
		mat2.set_emissive(vec4(0.0f, 0.0f, 0.0f, 1.0f));
		//specular
		mat2.set_specular(vec4(0.1f));
		//shininess
		mat2.set_shininess(0.1f);

		material mat3;
		//emissive
		mat2.set_emissive(vec4(0.0f, 0.0f, 0.0f, 1.0f));
		//specular
		mat2.set_specular(vec4(0.5f));
		//shininess
		mat2.set_shininess(0.0f);

		//table mat
		meshes["table"].set_material(mat);
		//bottle mat
		meshes["bottle1"].set_material(mat);
		meshes["box"].set_material(mat3);
		

		//skull mat
		meshes["skull"].set_material(mat2);
		//plane mat
		meshes["plane"].set_material(mat3);
		//chair mat
		meshes["chair"].set_material(mat3);

	}

	// Load texture
	{
		tex["floor"] = texture("textures/stone.jpg", true, true);
		tex["skull"] = texture("textures/color.png");
		tex["table1"] = texture("textures/wood001.jpg");
		tex["bottle1"] = texture("textures/Transparency.jpg");
		tex["box"] = texture("textures/cardboard.jpg");
		tex["chair"] = texture("textures/wood1.jpg");
	
	}

	//Lights
	{
		// Set lighting values
		// controlable Point light
		points[0].set_position(vec3(-25.0f, 25.0f, -15.0f));
		points[0].set_light_colour(vec4(1.0f, 1.0f, 1.0f, 1.0f));
		points[0].set_range(20.0f);
		//default light
		points[1].set_position(vec3(-25.0f, 15.0f, -15.0f));
		points[1].set_light_colour(vec4(1.0f, 1.0f, 0.0f, 1.0f));
		points[1].set_range(20.0f);
		
		//spot light
		spots[0].set_position(vec3(-25.0f, 20.0f, -15.0f));
		spots[0].set_light_colour(vec4(1.0f, 1.0f, 1.0f, 1.0f));
		spots[0].set_direction(normalize(vec3(1.0f, -1.0f, -1.0f)));
		spots[0].set_range(20.0f);
		spots[0].set_power(0.5);
	
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
		array<string, 6> filenames = { "textures/posx.jpg","textures/negx.jpg","textures/posy.jpg","textures/negy.jpg","textures/posz.jpg","textures/negz.jpg" };
		sky_cube = cubemap(filenames);
	}
	{
		// Set target camera properties
		cam.set_position(vec3(50.0f, 10.0f, 50.0f));
		cam.set_target(vec3(-25.0f, 6.0f, -25.0f));
		cam.set_projection(quarter_pi<float>(), renderer::get_screen_aspect(), 0.1f, 4000.0f);
		
		// Set free camera properties
		fcam.set_position(vec3(50.0f, 10.0f, 50.0f));
		fcam.set_target(vec3(0.0f, 0.0f, 0.0f));
		fcam.set_projection(quarter_pi<float>(), renderer::get_screen_aspect(), 0.1f, 4000.0f);
		
	}
	return true;
}

bool update(float delta_time)
{
	//smoke
	///////////////////////////////////////////////////////
	if (delta_time > 10.0f) {
		delta_time = 10.0f;
	}
	renderer::bind(compute_eff);
	glUniform3fv(compute_eff.get_uniform_location("max_dims"), 1, &(vec3(3.0f, 5.0f, 5.0f))[0]);
	glUniform1f(compute_eff.get_uniform_location("delta_time"), delta_time);
	///////////////////////////////////////////////////////////

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

	//smoke
	/////////////////////////////////////////////
	{
		// Bind Compute Shader
		renderer::bind(compute_eff);
		// Bind data as SSBO
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, G_Position_buffer);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, G_Velocity_buffer);
		// Dispatch
		glDispatchCompute(MAX_PARTICLES / 128, 1, 1);
		// Sync, wait for completion
		glMemoryBarrier(GL_ALL_BARRIER_BITS);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

		// *********************************
		// Bind render effect
		renderer::bind(smoke_eff);
		// Create MV matrix
		mat4 M(1.0f);
		auto V = cam.get_view();
		auto MV = V * M;
		// Set the colour uniform
		glUniform4fv(smoke_eff.get_uniform_location("colour"), 1, value_ptr(vec4(1.0f)));
		// Set MV, and P matrix uniforms seperatly
		glUniformMatrix4fv(smoke_eff.get_uniform_location("MV"), 1, GL_FALSE, value_ptr(MV));
		glUniformMatrix4fv(smoke_eff.get_uniform_location("P"), 1, GL_FALSE, value_ptr(cam.get_projection()));
		// Set point_size size uniform to .1f
		glUniform1f(smoke_eff.get_uniform_location("point_size"), 0.1f);
		// Bind particle texture
		renderer::bind(smoke_tex, 0);
		// *********************************

		// Bind position buffer as GL_ARRAY_BUFFER
		glBindBuffer(GL_ARRAY_BUFFER, G_Position_buffer);
		// Setup vertex format
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (void *)0);
		// Enable Blending
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		// Disable Depth Mask
		glDepthMask(GL_FALSE);
		// Render
		glDrawArrays(GL_POINTS, 0, MAX_PARTICLES);
		// Tidy up, enable depth mask
		glDepthMask(GL_TRUE);
		// Disable Blend
		glDisable(GL_BLEND);
		// Unbind all arrays
		glDisableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glUseProgram(0);
	}
	//////////////////////////////////////////

	 
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

		
		
		// Bind point lights
		renderer::bind(points, "points");
		// Bind spot lights
		renderer::bind(spots, "spots");

		// Bind textures
		if (e.first == "skull")
		{
			renderer::bind(m.get_material(), "mat2");
			renderer::bind(tex["skull"], 0);
			glUniform1i(eff.get_uniform_location("tex"), 0);
			
		}
		if (e.first == "chair")
		{
			renderer::bind(m.get_material(), "mat3");
			renderer::bind(tex["chair"], 0);
			glUniform1i(eff.get_uniform_location("tex"), 0);

		}
		if (e.first == "box")
		{
			renderer::bind(m.get_material(), "mat");
			renderer::bind(tex["box"], 0);
			glUniform1i(eff.get_uniform_location("tex"), 0);

		}
		if (e.first == "bottle1")
		{
			renderer::bind(m.get_material(), "mat");
			renderer::bind(tex["bottle1"], 0);
			glUniform1i(eff.get_uniform_location("tex"), 0);

		}
		
		if (e.first == "table")
		{
		//	renderer::bind(tex["table1"], 1);
			renderer::bind(m.get_material(), "mat3");
			renderer::bind(tex["table1"], 0);
			glUniform1i(eff.get_uniform_location("tex"), 0);
			
		}

		if (e.first == "plane")
		{
			// Bind material
			renderer::bind(m.get_material(), "mat");
			//bind texture
			renderer::bind(tex["floor"], 2);
			//set tex uniform
			glUniform1i(eff.get_uniform_location("tex"), 2);
			
		
			
		}

		

		// Set tex uniform
		//glUniform1i(eff.get_uniform_location("tex"), 0);
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