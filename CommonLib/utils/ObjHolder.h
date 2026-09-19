#pragma once

namespace CommonLib
{

	template<class TObj>
	class TObjHolder
	{
		public:
			TObjHolder(TObj& obj, std::function<void(TObj&)> funcDest) : m_obj(obj), m_funcDest(funcDest)
			{

			}

			~TObjHolder()
			{
				m_funcDest(m_obj);
			}

			TObj& Get() { return m_obj; }
			const TObj& Get() const { return m_obj; }

		protected:
			TObj m_obj;
			std::function<void(TObj&)> m_funcDest;
	};

	template<class TObj>
	class TObjHolderSafe
	{
	public:
		TObjHolderSafe(TObj& obj, std::function<void(TObj&)> funcDest) : m_obj(obj), m_funcDest(funcDest)
		{

		}

		~TObjHolderSafe()
		{
			try
			{
				m_funcDest(m_obj);
			}
			catch (std::exception& exe)
			{
				exe; //TO DO log
			}
		}

		TObj& Get() { return m_obj; }
		const TObj& Get() const { return m_obj; }

	protected:
		TObj m_obj;
		std::function<void(TObj&)> m_funcDest;
	};

}