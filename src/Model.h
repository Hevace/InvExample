// Definition of system model

#ifndef __MODEL_H__
#define __MODEL_H__

namespace inv_example {
// ================================================================================
// Model coefficients
// ================================================================================
const double MODEL_A[][4] = {
    { 1.0000, 0.0100, 0.0001, 0.0 },
    { 0.0, 0.9982, 0.0267, 0.0001 },
    { 0.0, 0.0, 1.0016, 0.0100 },
    { 0.0, -0.0045, 0.3119, 1.0016 }
};

const double MODEL_B[] = {
    0.0001,
    0.0182,
    0.0002,
    0.0454

};

const double MODEL_C[][] = {
    { 1, 0, 0, 0 },
    { 0, 0, 1, 0 }
};

const double CTL_K[] = {
    -61.9933, -33.5040, 95.0597, 18.8300
};

const double CTL_NBAR = -61.5500;


// ================================================================================
// Cart model
// ================================================================================
class CartModel
{
public: // constructor
    CartModel(void) : m_force_cmd(0), m_pos(0), m_vel(0) {};
    CartModel(double pos_init, double vel_init) : m_force_cmd(0), m_pos(pos_init), m_vel(vel_init) {};

public: // methods
    void set_force_cmd(double force) { m_force_cmd = force; };
    double get_force_cmd(void) { return m_force_cmd; };
    void set_pos_vel(double pos, double vel) { m_pos = pos; m_vel = vel; };
    double get_pos(void) { return m_pos; };
    double get_vel(void) { return m_vel; };
    // TODO add methods for cart model communications, reponse to polling, TCP server, etc.

private: // data
    double m_force_cmd;
    double m_pos;
    double m_vel;
};


// ================================================================================
// Pendulum model
// ================================================================================
class PendModel
{
public: // constructor
    PendModel(void) : m_pos(0), m_vel(0) {};
    PendModel(double pos_init, double vel_init) : m_pos(pos_init), m_vel(vel_init) {};

public: // methods
    double get_pos(void) { return m_pos; };
    double get_vel(void) { return m_vel; };
    void set_pos_vel(double pos, double vel);   // TODO set new state and trigger a UDP sensor message output

private: // data
    double m_pos;
    double m_vel;
};


// ================================================================================
// System math model
// ================================================================================
class InvPendModel
{
public: // types
    struct Inputs {
        double cart_force;
    };

    struct Outputs {
        double cart_pos;
        double pend_pos;
    };

    struct States {
        double cart_pos;
        double cart_vel;
        double pend_pos;
        double pend_vel;
    };

public: // constructors
    InvPendModel();

public: // methods
    Outputs iterate_100hz(Inputs in);           // TODO calculate system response
    void on_tick_100hz(double cart_force_cmd);  // TODO calculate response, update the cart model, update the pend model

private: // data
    States m_x;
    CartModel m_cart;
    PendModel m_pend;
};

} // namespace inv_example

#endif // __MODEL_H__
