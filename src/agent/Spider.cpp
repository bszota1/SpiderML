#include "Spider.hpp"

Spider::Spider(const Genome& genome, const Vector2& position, b2WorldId worldId) : 
    genome_{genome},
    neuralNetwork_{genome},
    worldId_{worldId},
    bodyPosition_{position},
    isAlive_{true},
    fitness_{0.0f}{
        buildBody(position);
    }

void Spider::kill(){
    isAlive_ = false;
}

void Spider::buildBody(const Vector2& position) {
    b2Filter spiderFilter = b2DefaultFilter();
    
    spiderFilter.categoryBits = 0x0002; 
    spiderFilter.maskBits = 0x0001;

    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.type = b2_dynamicBody;
    bodyDef.position = {position.x, position.y};
    torso_ = b2CreateBody(worldId_, &bodyDef);

    b2Polygon boxShape = b2MakeBox(15.0f, 10.0f);
    b2ShapeDef shapeDef = b2DefaultShapeDef();
    shapeDef.density = 1.0f;
    shapeDef.material.friction = 0.5f;
    shapeDef.filter = spiderFilter;

    b2CreatePolygonShape(torso_, &shapeDef, &boxShape);

    int legsPerSide = 2;
    float legHalfWidth = 20.0f;
    float legHalfHeight = 3.0f;

    
    for (int side = -1; side <= 1; side += 2) {
        for (int i = 0; i < legsPerSide; i++) {
            float yOffset = -8.0f + (i * 16.0f / (legsPerSide - 1));
            float xOffset = side * 15.0f;

            b2BodyDef thighDef = b2DefaultBodyDef();
            thighDef.type = b2_dynamicBody;
            thighDef.position = {position.x + side * (15.0f + 10.0f), position.y + yOffset};
            b2BodyId thighId = b2CreateBody(worldId_, &thighDef);
            legs_.push_back(thighId);

            b2Polygon thighShape = b2MakeBox(10.0f, 3.0f);
            b2ShapeDef thighShapeDef = b2DefaultShapeDef();
            thighShapeDef.density = 0.5f;
            thighShapeDef.filter = spiderFilter;
            b2CreatePolygonShape(thighId, &thighShapeDef, &thighShape);

            b2BodyDef calfDef = b2DefaultBodyDef();
            calfDef.type = b2_dynamicBody;
            calfDef.position = {position.x + side * (15.0f + 20.0f + 10.0f), position.y + yOffset};
            b2BodyId calfId = b2CreateBody(worldId_, &calfDef);
            legs_.push_back(calfId);

            b2Polygon calfShape = b2MakeBox(10.0f, 3.0f);
            b2ShapeDef calfShapeDef = b2DefaultShapeDef();
            calfShapeDef.density = 0.5f;
            calfShapeDef.material.friction = 0.9f;
            calfShapeDef.filter = spiderFilter;
            b2CreatePolygonShape(calfId, &calfShapeDef, &calfShape);

            b2RevoluteJointDef hipDef = b2DefaultRevoluteJointDef();
            hipDef.bodyIdA = torso_;
            hipDef.bodyIdB = thighId;
            hipDef.localAnchorA = {xOffset, yOffset};
            hipDef.localAnchorB = {-side * 10.0f, 0.0f};
            
            hipDef.enableMotor = true;
            hipDef.maxMotorTorque = 500000.0f;
            hipDef.motorSpeed = (i % 2 == 0) ? 3.0f : -3.0f;
            
            joints_.push_back(b2CreateRevoluteJoint(worldId_, &hipDef));

            b2RevoluteJointDef kneeDef = b2DefaultRevoluteJointDef();
            kneeDef.bodyIdA = thighId;
            kneeDef.bodyIdB = calfId;
            kneeDef.localAnchorA = {side * 10.0f, 0.0f};
            kneeDef.localAnchorB = {-side * 10.0f, 0.0f};
            
            kneeDef.enableMotor = true;
            kneeDef.maxMotorTorque = 400000.0f;
            kneeDef.motorSpeed = (i % 2 == 0) ? -4.0f : 4.0f;
            
            joints_.push_back(b2CreateRevoluteJoint(worldId_, &kneeDef));
        }
    }
}

void Spider::draw() const {
    b2Vec2 torsoPos = b2Body_GetPosition(torso_);
    b2Rot torsoRot = b2Body_GetRotation(torso_);
    float torsoAngle = b2Rot_GetAngle(torsoRot) * RAD2DEG;
    
    Rectangle torsoRect = {torsoPos.x, torsoPos.y, 30.0f, 20.0f};
    Vector2 torsoOrigin = {15.0f, 10.0f};
    
    DrawRectanglePro(torsoRect, torsoOrigin, torsoAngle, DARKGRAY);

    for (b2BodyId leg : legs_) {
        b2Vec2 legPos = b2Body_GetPosition(leg);
        b2Rot legRot = b2Body_GetRotation(leg);
        float legAngle = b2Rot_GetAngle(legRot) * RAD2DEG;
        
        Rectangle legRect = {legPos.x, legPos.y, 20.0f, 6.0f};
        Vector2 legOrigin = {10.0f, 3.0f}; 
        DrawRectanglePro(legRect, legOrigin, legAngle, RED);
    }
}

void Spider::update() {
    if (!isAlive_) return;

    std::vector<float> inputs;

    
    b2Rot torsoRot = b2Body_GetRotation(torso_);
    b2Vec2 velocity = b2Body_GetLinearVelocity(torso_);
    inputs.push_back(velocity.x * 0.1f);
    inputs.push_back(velocity.y * 0.1f);
    inputs.push_back(b2Rot_GetAngle(torsoRot));

    
    for (b2JointId joint : joints_) {
        inputs.push_back(b2RevoluteJoint_GetAngle(joint));
    }

    double time = GetTime();
    double stime = sinf(time * 5.0f);
    double ctime = cosf(time * 5.0f);

    inputs.push_back(stime);
    inputs.push_back(ctime);

    b2Vec2 position = b2Body_GetPosition(torso_);
    float height = (550.0f - position.y) * 0.01f;
    inputs.push_back(height);

    std::vector<float> outputs = neuralNetwork_.feedForward(inputs);

   
    for (size_t i = 0; i < joints_.size(); i++) {
        float outputValue = (i < outputs.size()) ? outputs[i] : 0.0f;
        float desiredSpeed = (outputValue * 30.0f) - 15.0f;
        b2RevoluteJoint_SetMotorSpeed(joints_[i], desiredSpeed);
    }
    b2Vec2 pos = b2Body_GetPosition(torso_);
    fitness_ = pos.x; 
}


Spider::~Spider(){
    if (b2Body_IsValid(torso_)) {
        b2DestroyBody(torso_);
    }

    for (b2BodyId leg : legs_) {
        if (b2Body_IsValid(leg)) {
            b2DestroyBody(leg);
        }
    }
}